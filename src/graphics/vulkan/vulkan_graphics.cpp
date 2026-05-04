#include "common.h"
#include "moth_graphics/graphics/vulkan/vulkan_graphics.h"
#include "moth_graphics/graphics/vulkan/vulkan_command_buffer.h"
#include "moth_graphics/graphics/vulkan/vulkan_font.h"
#include "moth_graphics/graphics/vulkan/vulkan_texture.h"
#include "moth_graphics/graphics/vulkan/vulkan_utils.h"
#include "stb_image_write.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

namespace moth_graphics::graphics::vulkan {
    Graphics::Graphics(SurfaceContext& context, VkSurfaceKHR surface, uint32_t surfaceWidth, uint32_t surfaceHeight)
        : m_surfaceContext(context)
        , m_vkSurface(surface)
        , m_vkPipelineCache(VK_NULL_HANDLE) {
        CreateRenderPass();
        CreateShaders();
        CreateDefaultImage();

        VkPipelineCacheCreateInfo cacheInfo{};
        cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        CHECK_VK_RESULT(vkCreatePipelineCache(m_surfaceContext.GetVkDevice(), &cacheInfo, nullptr, &m_vkPipelineCache));

        m_swapchain = std::make_unique<Swapchain>(m_surfaceContext, *m_renderPass, surface, VkExtent2D{ surfaceWidth, surfaceHeight });

        m_contextStack.push(nullptr);
    }

    Graphics::~Graphics() {
        vkDeviceWaitIdle(m_surfaceContext.GetVkDevice());

        if (m_imguiInitialized) {
            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }
        if (m_overrideContext.m_vertexBuffer != nullptr && m_overrideContext.m_vertexBufferData != nullptr) {
            m_overrideContext.m_vertexBuffer->Unmap();
            m_overrideContext.m_vertexBufferData = nullptr;
        }
        if (m_defaultContext.m_vertexBuffer != nullptr && m_defaultContext.m_vertexBufferData != nullptr) {
            m_defaultContext.m_vertexBuffer->Unmap();
            m_defaultContext.m_vertexBufferData = nullptr;
        }
        vkDestroyPipelineCache(m_surfaceContext.GetVkDevice(), m_vkPipelineCache, nullptr);
    }

    bool Graphics::Begin() {
        m_defaultContext.m_target = m_swapchain->GetNextFramebuffer();
        if (m_defaultContext.m_target == nullptr) {
            // Swapchain is out-of-date (e.g. window was resized between Update and
            // Draw).  Query the actual surface size from Vulkan, skip recreation
            // entirely when the window is minimised (0×0 extent), and retry once.
            VkSurfaceCapabilitiesKHR caps{};
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                m_surfaceContext.GetVkPhysicalDevice(), m_vkSurface, &caps);
            if (caps.currentExtent.width == 0 || caps.currentExtent.height == 0) {
                return false;
            }
            OnResize(m_vkSurface, caps.currentExtent.width, caps.currentExtent.height);
            m_defaultContext.m_target = m_swapchain->GetNextFramebuffer();
            if (m_defaultContext.m_target == nullptr) {
                return false;
            }
        }

        if (m_imguiInitialized) {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        VkFence cmdFence = m_defaultContext.m_target->GetFence().GetVkFence();
        vkResetFences(m_surfaceContext.GetVkDevice(), 1, &cmdFence);

        BeginContext(&m_defaultContext);
        return true;
    }

    void Graphics::End() {
        if (m_imguiInitialized) {
            ImGui::Render();
            if ((ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0) {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
            if (ImDrawData* drawData = ImGui::GetDrawData()) {
                FlushPendingBatch();
                ImGui_ImplVulkan_RenderDrawData(drawData, GetCurrentCommandBuffer()->GetVkCommandBuffer());
            }
        }

        EndContext();

        VkSemaphore waitSemaphores[] = { m_defaultContext.m_target->GetRenderFinishedSemaphore() };
        VkSwapchainKHR swapChains[] = { m_swapchain->GetVkSwapchain() };
        uint32_t swapchainIndices[] = { m_defaultContext.m_target->GetSwapchainIndex() };

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = waitSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = swapchainIndices;
        VkResult presentResult = vkQueuePresentKHR(m_surfaceContext.GetVkQueue(), &presentInfo);
        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
            // Swapchain is no longer optimal.  The next Begin() will detect the
            // out-of-date condition via vkAcquireNextImageKHR and recreate it.
            spdlog::warn("Vulkan: swapchain present returned {} — swapchain will be recreated on next frame",
                         static_cast<int>(presentResult));
        } else if (presentResult != VK_SUCCESS) {
            spdlog::error("Vulkan: vkQueuePresentKHR failed: {}", static_cast<int>(presentResult));
        }
    }

    void Graphics::SetBlendMode(BlendMode mode) {
        auto* context = CurrentContext();
        context->m_currentBlendMode = mode;
    }

    void Graphics::SetColor(Color const& color) {
        auto* context = CurrentContext();
        context->m_currentColor = color;
    }

    void Graphics::Clear() {
        auto* context = CurrentContext();
        DrawFillRectF({ { 0, 0 }, { static_cast<float>(context->m_logicalExtent.width), static_cast<float>(context->m_logicalExtent.height) } });
    }

    FloatMat4x4 Graphics::CurrentTransform() const {
        return m_currentTransform;
    }

    void Graphics::SetTransform(FloatMat4x4 const& transform) {
        m_currentTransform = transform;
    }

    void Graphics::DrawImage(Image const& image, IntVec2 const& pos, FloatVec2 const& pivot) {
        auto const imageWidth = image.GetWidth();
        auto const imageHeight = image.GetHeight();
        auto const offsetX = static_cast<int>(static_cast<float>(imageWidth) * pivot.x);
        auto const offsetY = static_cast<int>(static_cast<float>(imageHeight) * pivot.y);
        IntRect destRect = MakeRect(pos.x, pos.y, imageWidth, imageHeight);
        DrawImage(image, destRect - IntVec2{ offsetX, offsetY }, nullptr);
    }

    void Graphics::DrawImage(Image const& image, IntRect const& destRect, IntRect const* sourceRect) {
        auto* context = CurrentContext();
        auto texture = std::dynamic_pointer_cast<Texture>(image.GetTexture());
        if (!texture) {
            return;
        }

        FloatRect fDestRect = static_cast<FloatRect>(destRect);

        FloatRect imageRect;
        if (sourceRect != nullptr) {
            imageRect = static_cast<FloatRect>(*sourceRect);
        } else {
            imageRect = MakeRect(0.0f, 0.0f, static_cast<float>(image.GetWidth()), static_cast<float>(image.GetHeight()));
        }

        FloatVec2 textureDimensions = FloatVec2{ texture->GetVkExtent().width, texture->GetVkExtent().height };
        imageRect += static_cast<FloatVec2>(image.GetSourceRect().topLeft);
        imageRect /= textureDimensions;

        auto const t = CurrentTransform();
        Vertex vertices[6];

        vertices[0].xy = t.TransformPoint({ fDestRect.topLeft.x, fDestRect.topLeft.y });
        vertices[0].uv = { imageRect.topLeft.x, imageRect.topLeft.y };
        vertices[0].color = context->m_currentColor;
        vertices[1].xy = t.TransformPoint({ fDestRect.bottomRight.x, fDestRect.topLeft.y });
        vertices[1].uv = { imageRect.bottomRight.x, imageRect.topLeft.y };
        vertices[1].color = context->m_currentColor;
        vertices[2].xy = t.TransformPoint({ fDestRect.topLeft.x, fDestRect.bottomRight.y });
        vertices[2].uv = { imageRect.topLeft.x, imageRect.bottomRight.y };
        vertices[2].color = context->m_currentColor;

        vertices[3].xy = t.TransformPoint({ fDestRect.topLeft.x, fDestRect.bottomRight.y });
        vertices[3].uv = { imageRect.topLeft.x, imageRect.bottomRight.y };
        vertices[3].color = context->m_currentColor;
        vertices[4].xy = t.TransformPoint({ fDestRect.bottomRight.x, fDestRect.bottomRight.y });
        vertices[4].uv = { imageRect.bottomRight.x, imageRect.bottomRight.y };
        vertices[4].color = context->m_currentColor;
        vertices[5].xy = t.TransformPoint({ fDestRect.bottomRight.x, fDestRect.topLeft.y });
        vertices[5].uv = { imageRect.bottomRight.x, imageRect.topLeft.y };
        vertices[5].color = context->m_currentColor;

        VkDescriptorSet descriptorSet = m_drawingShader->GetDescriptorSet(*texture);
        SubmitVertices(vertices, 6, ETopologyType::Triangles, descriptorSet);
    }

    void Graphics::DrawImageTiled(Image const& image, IntRect const& destRect, IntRect const* sourceRect, float scale) {
        IntRect const imageRect = MakeRect(0, 0, image.GetWidth(), image.GetHeight());
        if (sourceRect == nullptr) {
            sourceRect = &imageRect;
        }
        auto const imageWidth = static_cast<int>(static_cast<float>(sourceRect->w()) * scale);
        auto const imageHeight = static_cast<int>(static_cast<float>(sourceRect->h()) * scale);
        if (imageWidth <= 0 || imageHeight <= 0) {
            return;
        }
        for (auto y = destRect.topLeft.y; y < destRect.bottomRight.y; y += imageHeight) {
            for (auto x = destRect.topLeft.x; x < destRect.bottomRight.x; x += imageWidth) {
                IntRect const tiledDstRect{ { x, y }, { x + imageWidth, y + imageHeight } };
                DrawImage(image, tiledDstRect, sourceRect);
            }
        }
    }

    void Graphics::DrawRectF(FloatRect const& rect) {
        auto* context = CurrentContext();
        auto const t = CurrentTransform();
        auto const tl = t.TransformPoint({ rect.topLeft.x, rect.topLeft.y });
        auto const tr = t.TransformPoint({ rect.bottomRight.x, rect.topLeft.y });
        auto const br = t.TransformPoint({ rect.bottomRight.x, rect.bottomRight.y });
        auto const bl = t.TransformPoint({ rect.topLeft.x, rect.bottomRight.y });
        Vertex vertices[8];

        vertices[0].xy = tl;
        vertices[0].uv = { 0, 0 };
        vertices[0].color = context->m_currentColor;
        vertices[1].xy = tr;
        vertices[1].uv = { 0, 0 };
        vertices[1].color = context->m_currentColor;
        vertices[2].xy = tr;
        vertices[2].uv = { 0, 0 };
        vertices[2].color = context->m_currentColor;
        vertices[3].xy = br;
        vertices[3].uv = { 0, 0 };
        vertices[3].color = context->m_currentColor;
        vertices[4].xy = br;
        vertices[4].uv = { 0, 0 };
        vertices[4].color = context->m_currentColor;
        vertices[5].xy = bl;
        vertices[5].uv = { 0, 0 };
        vertices[5].color = context->m_currentColor;
        vertices[6].xy = bl;
        vertices[6].uv = { 0, 0 };
        vertices[6].color = context->m_currentColor;
        vertices[7].xy = tl;
        vertices[7].uv = { 0, 0 };
        vertices[7].color = context->m_currentColor;

        SubmitVertices(vertices, 8, ETopologyType::Lines);
    }

    void Graphics::DrawFillRectF(FloatRect const& rect) {
        auto* context = CurrentContext();
        auto const t = CurrentTransform();
        auto const tl = t.TransformPoint({ rect.topLeft.x, rect.topLeft.y });
        auto const tr = t.TransformPoint({ rect.bottomRight.x, rect.topLeft.y });
        auto const br = t.TransformPoint({ rect.bottomRight.x, rect.bottomRight.y });
        auto const bl = t.TransformPoint({ rect.topLeft.x, rect.bottomRight.y });
        Vertex vertices[6];

        vertices[0].xy = tl;
        vertices[0].uv = { 0, 0 };
        vertices[0].color = context->m_currentColor;
        vertices[1].xy = tr;
        vertices[1].uv = { 0, 0 };
        vertices[1].color = context->m_currentColor;
        vertices[2].xy = bl;
        vertices[2].uv = { 0, 0 };
        vertices[2].color = context->m_currentColor;
        vertices[3].xy = bl;
        vertices[3].uv = { 0, 0 };
        vertices[3].color = context->m_currentColor;
        vertices[4].xy = br;
        vertices[4].uv = { 0, 0 };
        vertices[4].color = context->m_currentColor;
        vertices[5].xy = tr;
        vertices[5].uv = { 0, 0 };
        vertices[5].color = context->m_currentColor;

        SubmitVertices(vertices, 6, ETopologyType::Triangles);
    }

    void Graphics::DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) {
        auto* context = CurrentContext();
        auto const t = CurrentTransform();
        Vertex vertices[2];

        vertices[0].xy = t.TransformPoint(p0);
        vertices[0].uv = { 0, 0 };
        vertices[0].color = context->m_currentColor;
        vertices[1].xy = t.TransformPoint(p1);
        vertices[1].uv = { 0, 0 };
        vertices[1].color = context->m_currentColor;

        SubmitVertices(vertices, 2, ETopologyType::Lines);
    }

    void Graphics::DrawText(std::string_view text, IFont& font, IntRect const& destRect, TextHorizAlignment horizontalAlignment, TextVertAlignment verticalAlignment) {
        std::string const textStr(text);
        auto* context = CurrentContext();
        context->m_currentBlendMode = BlendMode::Alpha; // force alpha blending for text
        Font& vulkanFont = dynamic_cast<Font&>(font);

        uint32_t const glyphStart = context->m_glyphCount;
        FontGlyphInstance* glyphInstances = static_cast<FontGlyphInstance*>(context->m_fontInstanceStagingBuffer->Map());

        auto const t = CurrentTransform();
        float const rotationRad = t.GetRotationDegrees() * kDegToRad;
        // use this to actually submit characters at a position
        auto SubmitCharacter = [&](uint32_t glyphIndex, FloatVec2 const& pos) {
            if (context->m_glyphCount >= 1024) {
                spdlog::warn("DrawText: glyph buffer full (1024 limit); remaining glyphs will not be rendered");
                return;
            }

            FontGlyphInstance* inst = &glyphInstances[context->m_glyphCount];
            inst->pos = pos;
            inst->glyphIndex = glyphIndex;
            inst->rotation = rotationRad;
            inst->color = context->m_currentColor;

            context->m_glyphCount++;
        };

        auto const lines = vulkanFont.WrapString(textStr, destRect.w());
        auto const singleLineHeight = vulkanFont.GetLineHeight();
        auto const singleLineDescent = vulkanFont.GetDescent();
        auto const linesHeight = static_cast<int32_t>(lines.size() * singleLineHeight);

        FloatVec2 penPos = static_cast<FloatVec2>(destRect.topLeft);

        switch (verticalAlignment) {
        case TextVertAlignment::Top:
            break;
        case TextVertAlignment::Middle:
            penPos.y += static_cast<float>(destRect.h() - linesHeight) / 2.0f;
            break;
        case TextVertAlignment::Bottom:
            penPos.y += static_cast<float>(destRect.h() - linesHeight);
            break;
        }

        // move down to the bottom of the line, minus the descent value (so the descent of the glyphs dont extend past the whole line)
        penPos.y += static_cast<float>(singleLineHeight + singleLineDescent);

        // render lines one by one
        for (const auto& line : lines) {
            auto const shapeInfo = vulkanFont.ShapeString(line.text);

            switch (horizontalAlignment) {
            case TextHorizAlignment::Left:
                penPos.x = static_cast<float>(destRect.topLeft.x);
                break;
            case TextHorizAlignment::Center:
                penPos.x = static_cast<float>(destRect.topLeft.x) + (static_cast<float>(destRect.w() - line.lineWidth) / 2.0f);
                break;
            case TextHorizAlignment::Right:
                penPos.x = static_cast<float>(destRect.bottomRight.x) - static_cast<float>(line.lineWidth);
                break;
            }

            for (auto const& info : shapeInfo) {
                if (info.glyphIndex >= 0) {
                    auto const bearing = static_cast<FloatVec2>(vulkanFont.GetGlyphBearing(info.glyphIndex));
                    auto const offset = static_cast<FloatVec2>(info.offset);
                    auto const glyphPos = t.TransformPoint(penPos + bearing + offset);
                    SubmitCharacter(static_cast<uint32_t>(info.glyphIndex), glyphPos);
                }
                penPos.x += static_cast<float>(info.advance.x);
            }

            penPos.y += static_cast<float>(singleLineHeight);
        }

        context->m_fontInstanceStagingBuffer->Unmap();

        uint32_t const glyphCount = context->m_glyphCount - glyphStart;
        if (glyphCount != 0u) {
            auto& commandBuffer = context->m_target->GetCommandBuffer();

            FlushPendingBatch();
            commandBuffer.BindVertexBuffer(*context->m_fontInstanceBuffer, 0);

            auto const& pipeline = GetCurrentFontPipeline();
            if (context->m_currentPipelineId != pipeline.m_hash) {
                commandBuffer.BindPipeline(pipeline);
                context->m_currentPipelineId = pipeline.m_hash;
            }

            commandBuffer.BindDescriptorSet(*m_fontShader, vulkanFont.GetVKDescriptorSetForShader(*m_fontShader), 0);
            commandBuffer.Draw(4, 0, glyphCount, glyphStart);
        }
    }

    void Graphics::SetClip(IntRect const* clipRect) {
        FlushPendingBatch();
        auto* context = CurrentContext();
        auto& commandBuffer = context->m_target->GetCommandBuffer();
        if (clipRect != nullptr) {
            VkRect2D scissor;
            scissor.offset.x = clipRect->x();
            scissor.offset.y = clipRect->y();
            scissor.extent.width = clipRect->w();
            scissor.extent.height = clipRect->h();
            commandBuffer.SetScissor(scissor);
        } else {
            VkRect2D scissor;
            scissor.offset.x = 0;
            scissor.offset.y = 0;
            scissor.extent = context->m_target->GetVkExtent();
            commandBuffer.SetScissor(scissor);
        }
    }

    std::unique_ptr<ITarget> Graphics::CreateTarget(int width, int height) {
        return std::make_unique<Framebuffer>(m_surfaceContext, width, height, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, m_rtRenderPass->GetRenderPass());
    }

    bool Graphics::IsRenderTarget() const {
        return m_contextStack.top() == &m_overrideContext;
    }

    ITarget* Graphics::GetTarget() {
        return m_overrideContext.m_target;
    }

    void Graphics::SetTarget(ITarget* target) {
        if (IsRenderTarget()) {
            EndContext();
        }

        if (target != nullptr) {
            m_overrideContext.m_target = dynamic_cast<Framebuffer*>(target);
            assert(m_overrideContext.m_target);
            VkFence fence = m_overrideContext.m_target->GetFence().GetVkFence();
            vkWaitForFences(m_surfaceContext.GetVkDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
            vkResetFences(m_surfaceContext.GetVkDevice(), 1, &fence);
            BeginContext(&m_overrideContext);
        }
    }

    void Graphics::SetLogicalSize(IntVec2 const& logicalSize) {
        auto* context = CurrentContext();
        auto& commandBuffer = context->m_target->GetCommandBuffer();
        PushConstants constants;
        constants.xyScale = { 2.0f / static_cast<float>(logicalSize.x), 2.0f / static_cast<float>(logicalSize.y) };
        constants.xyOffset = { -1.0f, -1.0f };
        commandBuffer.PushConstants(*m_drawingShader, VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstants), &constants);
        commandBuffer.PushConstants(*m_fontShader, VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstants), &constants);
    }

    void Graphics::OnResize(VkSurfaceKHR surface, uint32_t surfaceWidth, uint32_t surfaceHeight) {
        vkDeviceWaitIdle(m_surfaceContext.GetVkDevice());
        m_swapchain.reset();
        m_swapchain = std::make_unique<Swapchain>(m_surfaceContext, *m_renderPass, surface, VkExtent2D{ surfaceWidth, surfaceHeight });
    }
}

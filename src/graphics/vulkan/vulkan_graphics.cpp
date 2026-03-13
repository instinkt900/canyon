#include "common.h"
#include "canyon/graphics/vulkan/vulkan_graphics.h"
#include "canyon/graphics/vulkan/vulkan_command_buffer.h"
#include "canyon/graphics/vulkan/vulkan_font.h"
#include "canyon/graphics/vulkan/vulkan_texture.h"
#include "canyon/graphics/vulkan/vulkan_utils.h"
#include "stb_image_write.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

namespace canyon::graphics::vulkan {
    Graphics::Graphics(SurfaceContext& context, VkSurfaceKHR surface, uint32_t surfaceWidth, uint32_t surfaceHeight)
        : m_surfaceContext(context)
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

    void Graphics::Begin() {
        if (m_imguiInitialized) {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        m_defaultContext.m_target = m_swapchain->GetNextFramebuffer();
        VkFence cmdFence = m_defaultContext.m_target->GetFence().GetVkFence();
        vkResetFences(m_surfaceContext.GetVkDevice(), 1, &cmdFence);

        BeginContext(&m_defaultContext);
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
        vkQueuePresentKHR(m_surfaceContext.GetVkQueue(), &presentInfo);
    }

    void Graphics::SetBlendMode(BlendMode mode) {
        auto* context = CurrentContext();
        context->m_currentBlendMode = mode;
    }

    // void Graphics::SetBlendMode(std::shared_ptr<IImage> target, EBlendMode mode) {
    // }

    // void Graphics::SetColorMod(std::shared_ptr<IImage> target, Color const& color) {
    // }

    void Graphics::SetColor(Color const& color) {
        auto* context = CurrentContext();
        context->m_currentColor = color;
    }

    void Graphics::Clear() {
        auto* context = CurrentContext();
        DrawFillRectF({ { 0, 0 }, { static_cast<float>(context->m_logicalExtent.width), static_cast<float>(context->m_logicalExtent.height) } });
    }

    void Graphics::DrawImage(IImage& image, IntRect const& destRect, IntRect const* sourceRect, float rotation) {
        auto* context = CurrentContext();
        auto& vulkanImage = dynamic_cast<Image&>(image);
        auto texture = vulkanImage.m_texture;

        FloatRect fDestRect = static_cast<FloatRect>(destRect);

        FloatRect imageRect;
        if (sourceRect != nullptr) {
            imageRect = static_cast<FloatRect>(*sourceRect);
        } else {
            imageRect = MakeRect(0.0f, 0.0f, static_cast<float>(image.GetWidth()), static_cast<float>(image.GetHeight()));
        }

        FloatVec2 textureDimensions = FloatVec2{ texture->GetVkExtent().width, texture->GetVkExtent().height };
        imageRect += static_cast<FloatVec2>(vulkanImage.m_sourceRect.topLeft);
        imageRect /= textureDimensions;

        Vertex vertices[6];

        vertices[0].xy = { fDestRect.topLeft.x, fDestRect.topLeft.y };
        vertices[0].uv = { imageRect.topLeft.x, imageRect.topLeft.y };
        vertices[0].color = context->m_currentColor;
        vertices[1].xy = { fDestRect.bottomRight.x, fDestRect.topLeft.y };
        vertices[1].uv = { imageRect.bottomRight.x, imageRect.topLeft.y };
        vertices[1].color = context->m_currentColor;
        vertices[2].xy = { fDestRect.topLeft.x, fDestRect.bottomRight.y };
        vertices[2].uv = { imageRect.topLeft.x, imageRect.bottomRight.y };
        vertices[2].color = context->m_currentColor;

        vertices[3].xy = { fDestRect.topLeft.x, fDestRect.bottomRight.y };
        vertices[3].uv = { imageRect.topLeft.x, imageRect.bottomRight.y };
        vertices[3].color = context->m_currentColor;
        vertices[4].xy = { fDestRect.bottomRight.x, fDestRect.bottomRight.y };
        vertices[4].uv = { imageRect.bottomRight.x, imageRect.bottomRight.y };
        vertices[4].color = context->m_currentColor;
        vertices[5].xy = { fDestRect.bottomRight.x, fDestRect.topLeft.y };
        vertices[5].uv = { imageRect.bottomRight.x, imageRect.topLeft.y };
        vertices[5].color = context->m_currentColor;

        if (rotation != 0) {
            auto const center = static_cast<FloatVec2>(destRect.topLeft + IntVec2{ destRect.w() / 2, destRect.h() / 2 });
            for (int i = 0; i < 6; ++i) {
                auto const translated = Translate(vertices[i].xy, -center);
                auto const rotated = Rotate2D(translated, rotation);
                vertices[i].xy = Translate(rotated, center);
            }
        }

        VkDescriptorSet descriptorSet = m_drawingShader->GetDescriptorSet(*texture);
        SubmitVertices(vertices, 6, ETopologyType::Triangles, descriptorSet);
    }

    void Graphics::DrawImageTiled(graphics::IImage& image, IntRect const& destRect, IntRect const* sourceRect, float scale) {
        IntRect const imageRect = MakeRect(0, 0, image.GetWidth(), image.GetHeight());
        if (sourceRect == nullptr) {
            sourceRect = &imageRect;
        }
        auto const imageWidth = static_cast<int>(static_cast<float>(sourceRect->w()) * scale);
        auto const imageHeight = static_cast<int>(static_cast<float>(sourceRect->h()) * scale);
        for (auto y = destRect.topLeft.y; y < destRect.bottomRight.y; y += imageHeight) {
            for (auto x = destRect.topLeft.x; x < destRect.bottomRight.x; x += imageWidth) {
                IntRect const tiledDstRect{ { x, y }, { x + imageWidth, y + imageHeight } };
                DrawImage(image, tiledDstRect, sourceRect, 0);
            }
        }
    }

    void Graphics::DrawToPNG(IImage& image, std::filesystem::path const& path) {
        auto& srcVkImage = dynamic_cast<Image&>(image);
        auto srcTexture = srcVkImage.m_texture;
        auto const srcFormat = srcTexture->GetVkFormat();

        auto const targetWidth = static_cast<uint32_t>(image.GetWidth());
        auto const targetHeight = static_cast<uint32_t>(image.GetHeight());
        auto const targetFormat = VK_FORMAT_R8G8B8A8_UNORM;
        auto stagingImage = std::make_unique<Texture>(
            m_surfaceContext, targetWidth, targetHeight,
            targetFormat, VK_IMAGE_TILING_LINEAR,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        // Render target images are in SHADER_READ_ONLY_OPTIMAL after SetTarget(nullptr).
        VkImageLayout const srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        auto commandBuffer = std::make_unique<CommandBuffer>(m_surfaceContext);
        commandBuffer->BeginRecord();
        commandBuffer->TransitionImageLayout(*srcTexture, srcFormat, srcLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        commandBuffer->TransitionImageLayout(*stagingImage, targetFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        commandBuffer->CopyImageToImage(*srcTexture, *stagingImage);
        commandBuffer->TransitionImageLayout(*stagingImage, targetFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
        commandBuffer->TransitionImageLayout(*srcTexture, srcFormat, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcLayout);
        commandBuffer->SubmitAndWait();

        // Query the actual row pitch — linear images may have alignment padding
        // that makes rowPitch > width * bytesPerPixel.
        VkImageSubresource subresource{};
        subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        VkSubresourceLayout subLayout{};
        vkGetImageSubresourceLayout(m_surfaceContext.GetVkDevice(), stagingImage->GetVkImage(), &subresource, &subLayout);
        auto const rowPitch = static_cast<uint32_t>(subLayout.rowPitch);

        // Swizzle BGRA → RGBA row by row, respecting the actual row pitch.
        uint8_t const* data = static_cast<uint8_t const*>(stagingImage->Map());
        std::vector<uint8_t> dataCopy(targetWidth * targetHeight * 4);
        for (uint32_t row = 0; row < targetHeight; ++row) {
            uint8_t const* src = data + row * rowPitch;
            uint8_t* dst = dataCopy.data() + row * targetWidth * 4;
            for (uint32_t col = 0; col < targetWidth; ++col) {
                dst[col * 4 + 0] = src[col * 4 + 2];
                dst[col * 4 + 1] = src[col * 4 + 1];
                dst[col * 4 + 2] = src[col * 4 + 0];
                dst[col * 4 + 3] = src[col * 4 + 3];
            }
        }
        stbi_write_png(path.string().c_str(), static_cast<int>(targetWidth), static_cast<int>(targetHeight), 4, dataCopy.data(), static_cast<int>(targetWidth) * 4);
        stagingImage->Unmap();
    }

    void Graphics::DrawRectF(FloatRect const& rect) {
        auto* context = CurrentContext();
        Vertex vertices[8];

        vertices[0].xy = { rect.topLeft.x, rect.topLeft.y };
        vertices[0].uv = { 0, 0 };
        vertices[0].color = context->m_currentColor;
        vertices[1].xy = { rect.bottomRight.x, rect.topLeft.y };
        vertices[1].uv = { 0, 0 };
        vertices[1].color = context->m_currentColor;

        vertices[2].xy = { rect.bottomRight.x, rect.topLeft.y };
        vertices[2].uv = { 0, 0 };
        vertices[2].color = context->m_currentColor;
        vertices[3].xy = { rect.bottomRight.x, rect.bottomRight.y };
        vertices[3].uv = { 0, 0 };
        vertices[3].color = context->m_currentColor;

        vertices[4].xy = { rect.bottomRight.x, rect.bottomRight.y };
        vertices[4].uv = { 0, 0 };
        vertices[4].color = context->m_currentColor;
        vertices[5].xy = { rect.topLeft.x, rect.bottomRight.y };
        vertices[5].uv = { 0, 0 };
        vertices[5].color = context->m_currentColor;

        vertices[6].xy = { rect.topLeft.x, rect.bottomRight.y };
        vertices[6].uv = { 0, 0 };
        vertices[6].color = context->m_currentColor;
        vertices[7].xy = { rect.topLeft.x, rect.topLeft.y };
        vertices[7].uv = { 0, 0 };
        vertices[7].color = context->m_currentColor;

        SubmitVertices(vertices, 8, ETopologyType::Lines);
    }

    void Graphics::DrawFillRectF(FloatRect const& rect) {
        auto* context = CurrentContext();
        Vertex vertices[6];

        vertices[0].xy = { rect.topLeft.x, rect.topLeft.y };
        vertices[0].uv = { 0, 0 };
        vertices[0].color = context->m_currentColor;
        vertices[1].xy = { rect.bottomRight.x, rect.topLeft.y };
        vertices[1].uv = { 0, 0 };
        vertices[1].color = context->m_currentColor;
        vertices[2].xy = { rect.topLeft.x, rect.bottomRight.y };
        vertices[2].uv = { 0, 0 };
        vertices[2].color = context->m_currentColor;

        vertices[3].xy = { rect.topLeft.x, rect.bottomRight.y };
        vertices[3].uv = { 0, 0 };
        vertices[3].color = context->m_currentColor;
        vertices[4].xy = { rect.bottomRight.x, rect.bottomRight.y };
        vertices[4].uv = { 0, 0 };
        vertices[4].color = context->m_currentColor;
        vertices[5].xy = { rect.bottomRight.x, rect.topLeft.y };
        vertices[5].uv = { 0, 0 };
        vertices[5].color = context->m_currentColor;

        SubmitVertices(vertices, 6, ETopologyType::Triangles);
    }

    void Graphics::DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) {
        auto* context = CurrentContext();
        Vertex vertices[2];

        vertices[0].xy = { p0.x, p0.y };
        vertices[0].uv = { 0, 0 };
        vertices[0].color = context->m_currentColor;
        vertices[1].xy = { p1.x, p1.y };
        vertices[1].uv = { 0, 0 };
        vertices[1].color = context->m_currentColor;

        SubmitVertices(vertices, 2, ETopologyType::Lines);
    }

    void Graphics::DrawText(std::string const& text, IFont& font, IntRect const& destRect, TextHorizAlignment horizontalAlignment, TextVertAlignment verticalAlignment) {
        auto* context = CurrentContext();
        context->m_currentBlendMode = BlendMode::Alpha; // force alpha blending for text
        Font& vulkanFont = dynamic_cast<Font&>(font);

        uint32_t const glyphStart = context->m_glyphCount;
        FontGlyphInstance* glyphInstances = static_cast<FontGlyphInstance*>(context->m_fontInstanceStagingBuffer->Map());

        // use this to actually submit characters at a position
        auto SubmitCharacter = [&](uint32_t glyphIndex, FloatVec2 const& pos) {
            if (context->m_glyphCount >= 1024) {
                return;
            }

            FontGlyphInstance* inst = &glyphInstances[context->m_glyphCount];
            inst->pos = pos;
            inst->glyphIndex = glyphIndex;
            inst->color = context->m_currentColor;

            context->m_glyphCount++;
        };

        auto const lines = vulkanFont.WrapString(text, destRect.w());
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
                    auto const glyphPos = penPos + bearing + offset;
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

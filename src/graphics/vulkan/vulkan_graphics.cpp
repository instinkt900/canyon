#include "common.h"
#include "canyon/platform/glfw/glfw_window.h"
#include "canyon/graphics/vulkan/vulkan_graphics.h"
#include "canyon/graphics/color.h"
#include "canyon/graphics/vulkan/vulkan_command_buffer.h"
#include "canyon/graphics/vulkan/vulkan_texture.h"
#include "canyon/graphics/vulkan/vulkan_font.h"
#include "canyon/graphics/vulkan/vulkan_utils.h"
#include "canyon/graphics/stb_image_write.h"
#include "shaders/vulkan_shaders.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

namespace {
    void checkVkResult(VkResult err) {
        CHECK_VK_RESULT(err);
    }
}

namespace {
    VkVertexInputBindingDescription getVertexBindingDescription() {
        VkVertexInputBindingDescription vertexBindingDesc{};

        vertexBindingDesc.binding = 0;
        vertexBindingDesc.stride = sizeof(canyon::graphics::vulkan::Graphics::Vertex);
        vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return vertexBindingDesc;
    }

    std::vector<VkVertexInputAttributeDescription> getVertexAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescs(3);

        vertexAttributeDescs[0].binding = 0;
        vertexAttributeDescs[0].location = 0;
        vertexAttributeDescs[0].format = VK_FORMAT_R32G32_SFLOAT;
        vertexAttributeDescs[0].offset = offsetof(canyon::graphics::vulkan::Graphics::Vertex, xy);

        vertexAttributeDescs[1].binding = 0;
        vertexAttributeDescs[1].location = 1;
        vertexAttributeDescs[1].format = VK_FORMAT_R32G32_SFLOAT;
        vertexAttributeDescs[1].offset = offsetof(canyon::graphics::vulkan::Graphics::Vertex, uv);

        vertexAttributeDescs[2].binding = 0;
        vertexAttributeDescs[2].location = 2;
        vertexAttributeDescs[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        vertexAttributeDescs[2].offset = offsetof(canyon::graphics::vulkan::Graphics::Vertex, color);

        return vertexAttributeDescs;
    }

    VkVertexInputBindingDescription getFontVertexBindingDescription() {
        VkVertexInputBindingDescription vertexBindingDesc{};

        vertexBindingDesc.binding = 0;
        vertexBindingDesc.stride = sizeof(canyon::graphics::vulkan::Graphics::FontGlyphInstance);
        vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        return vertexBindingDesc;
    }

    std::vector<VkVertexInputAttributeDescription> getFontVertexAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescs(3);

        vertexAttributeDescs[0].binding = 0;
        vertexAttributeDescs[0].location = 0;
        vertexAttributeDescs[0].format = VK_FORMAT_R32G32_SFLOAT;
        vertexAttributeDescs[0].offset = offsetof(canyon::graphics::vulkan::Graphics::FontGlyphInstance, pos);

        vertexAttributeDescs[1].binding = 0;
        vertexAttributeDescs[1].location = 1;
        vertexAttributeDescs[1].format = VK_FORMAT_R32_UINT;
        vertexAttributeDescs[1].offset = offsetof(canyon::graphics::vulkan::Graphics::FontGlyphInstance, glyphIndex);

        vertexAttributeDescs[2].binding = 0;
        vertexAttributeDescs[2].location = 2;
        vertexAttributeDescs[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        vertexAttributeDescs[2].offset = offsetof(canyon::graphics::vulkan::Graphics::FontGlyphInstance, color);

        return vertexAttributeDescs;
    }
}

namespace canyon::graphics::vulkan {
    Graphics::Graphics(SurfaceContext& context, VkSurfaceKHR surface, uint32_t surfaceWidth, uint32_t surfaceHeight)
        : m_surfaceContext(context) {
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
        if (m_imguiInitialized) {
            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }
        if (m_overrideContext.m_vertexBuffer && m_overrideContext.m_vertexBufferData) {
            m_overrideContext.m_vertexBuffer->Unmap();
            m_overrideContext.m_vertexBufferData = nullptr;
        }
        if (m_defaultContext.m_vertexBuffer && m_defaultContext.m_vertexBufferData) {
            m_defaultContext.m_vertexBuffer->Unmap();
            m_defaultContext.m_vertexBufferData = nullptr;
        }
        vkDestroyPipelineCache(m_surfaceContext.GetVkDevice(), m_vkPipelineCache, nullptr);
    }

    void Graphics::InitImgui(canyon::platform::Window const& window) {
        if (m_imguiInitialized) {
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        auto& glfwWindow = static_cast<canyon::platform::glfw::Window const&>(window);

        ImGui_ImplGlfw_InitForVulkan(glfwWindow.GetGLFWWindow(), true);
        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = m_surfaceContext.GetContext().GetInstance();
        initInfo.PhysicalDevice = m_surfaceContext.GetVkPhysicalDevice();
        initInfo.Device = m_surfaceContext.GetVkDevice();
        initInfo.QueueFamily = m_surfaceContext.GetVkQueueFamily();
        initInfo.Queue = m_surfaceContext.GetVkQueue();
        initInfo.DescriptorPool = m_surfaceContext.GetVkDescriptorPool();
        initInfo.RenderPass = m_renderPass->GetRenderPass();
        initInfo.Subpass = 0;
        initInfo.MinImageCount = m_swapchain->GetImageCount();
        initInfo.ImageCount = m_swapchain->GetImageCount();
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        initInfo.Allocator = nullptr;
        initInfo.CheckVkResultFn = checkVkResult;
        ImGui_ImplVulkan_Init(&initInfo);
        ImGui_ImplVulkan_CreateFontsTexture();

        m_imguiInitialized = true;
    }

    void Graphics::Begin() {
        if (m_imguiInitialized) {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        m_defaultContext.m_target = m_swapchain->GetNextFramebuffer();
        VkFence cmdFence = m_defaultContext.m_target->GetFence().GetVkFence();
        vkWaitForFences(m_surfaceContext.GetVkDevice(), 1, &cmdFence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_surfaceContext.GetVkDevice(), 1, &cmdFence);

        BeginContext(&m_defaultContext);
    }

    void Graphics::End() {
        if (m_imguiInitialized) {
            ImGui::Render();
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
            if (ImDrawData* drawData = ImGui::GetDrawData()) {
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
        auto context = m_contextStack.top();
        context->m_currentBlendMode = mode;
    }

    // void Graphics::SetBlendMode(std::shared_ptr<IImage> target, EBlendMode mode) {
    // }

    // void Graphics::SetColorMod(std::shared_ptr<IImage> target, Color const& color) {
    // }

    void Graphics::SetColor(Color const& color) {
        auto context = m_contextStack.top();
        context->m_currentColor = color;
    }

    void Graphics::Clear() {
        auto context = m_contextStack.top();
        DrawFillRectF({ { 0, 0 }, { static_cast<float>(context->m_logicalExtent.width), static_cast<float>(context->m_logicalExtent.height) } });
    }

    void Graphics::DrawImage(IImage& image, IntRect const& destRect, IntRect const* sourceRect) {
        auto context = m_contextStack.top();
        auto& vulkanImage = dynamic_cast<Image&>(image);
        auto texture = vulkanImage.m_texture;

        FloatRect fDestRect = static_cast<FloatRect>(destRect);

        FloatRect imageRect;
        if (sourceRect) {
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

        VkDescriptorSet descriptorSet = m_drawingShader->GetDescriptorSet(*texture);
        SubmitVertices(vertices, 6, ETopologyType::Triangles, descriptorSet);
    }

    void Graphics::DrawImageTiled(graphics::IImage& image, IntRect const& destRect, IntRect const* sourceRect, float scale) {
        IntRect const imageRect = MakeRect(0, 0, image.GetWidth(), image.GetHeight());
        if (!sourceRect) {
            sourceRect = &imageRect;
        }
        auto const imageWidth = static_cast<int>(sourceRect->w() * scale);
        auto const imageHeight = static_cast<int>(sourceRect->h() * scale);
        for (auto y = destRect.topLeft.y; y < destRect.bottomRight.y; y += imageHeight) {
            for (auto x = destRect.topLeft.x; x < destRect.bottomRight.x; x += imageWidth) {
                IntRect const tiledDstRect{ { x, y }, { x + imageWidth, y + imageHeight } };
                DrawImage(image, tiledDstRect, sourceRect);
            }
        }
    }

    void Graphics::DrawToPNG(std::filesystem::path const& path) {
        FlushCommands();

        auto& context = m_surfaceContext;
        auto drawContext = m_contextStack.top();

        auto const targetFormat = VK_FORMAT_R8G8B8A8_UNORM;
        auto targetImage = std::make_unique<Texture>(context, drawContext->m_logicalExtent.width, drawContext->m_logicalExtent.height, targetFormat, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        auto renderedSubImage = static_cast<Image*>(drawContext->m_target->GetImage());
        auto srcImage = renderedSubImage->m_texture;
        auto srcFormat = srcImage->GetVkFormat();

        auto commandBuffer = std::make_unique<CommandBuffer>(context);
        commandBuffer->BeginRecord();
        commandBuffer->TransitionImageLayout(*srcImage, srcFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        commandBuffer->TransitionImageLayout(*targetImage, targetFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        commandBuffer->CopyImageToImage(*srcImage, *targetImage);
        commandBuffer->TransitionImageLayout(*targetImage, targetFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
        commandBuffer->TransitionImageLayout(*srcImage, srcFormat, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        commandBuffer->SubmitAndWait();

        // swizzle BGRA to RGBA
        // theres probably a way to do this on the gpu?
        auto const targetWidth = drawContext->m_logicalExtent.width;
        auto const targetHeight = drawContext->m_logicalExtent.height;
        uint8_t* data = static_cast<uint8_t*>(targetImage->Map());
        std::vector<uint8_t> dataCopy;
        dataCopy.resize(targetWidth * targetHeight * 4);
        uint8_t* dst = dataCopy.data();
        for (size_t j = 0; j < dataCopy.size(); j += 4) {
            dst[j + 0] = data[j + 2];
            dst[j + 1] = data[j + 1];
            dst[j + 2] = data[j + 0];
            dst[j + 3] = data[j + 3];
        }

        stbi_write_png(path.string().c_str(), targetWidth, targetHeight, 4, dst, targetWidth * 4);

        targetImage->Unmap();

        m_contextStack.pop();
    }

    void Graphics::DrawRectF(FloatRect const& rect) {
        auto context = m_contextStack.top();
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
        auto context = m_contextStack.top();
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
        auto context = m_contextStack.top();
        Vertex vertices[2];

        vertices[0].xy = { p0.x, p0.y };
        vertices[0].uv = { 0, 0 };
        vertices[0].color = context->m_currentColor;
        vertices[1].xy = { p1.x, p1.y };
        vertices[1].uv = { 0, 0 };
        vertices[1].color = context->m_currentColor;

        SubmitVertices(vertices, 2, ETopologyType::Lines);
    }

    void Graphics::DrawText(std::string const& text, IFont& font, TextHorizAlignment horizontalAlignment, TextVertAlignment verticalAlignment, IntRect const& destRect) {
        auto context = m_contextStack.top();
        context->m_currentBlendMode = BlendMode::Alpha; // force alpha blending for text
        Font& vulkanFont = static_cast<Font&>(font);

        uint32_t const glyphStart = context->m_glyphCount;
        FontGlyphInstance* glyphInstances = static_cast<FontGlyphInstance*>(context->m_fontInstanceStagingBuffer->Map());

        // use this to actually submit characters at a position
        auto SubmitCharacter = [&](uint32_t glyphIndex, FloatVec2 const& pos) {
            if (context->m_glyphCount >= 1024)
                return;

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
            penPos.y += (destRect.h() - linesHeight) / 2.0f;
            break;
        case TextVertAlignment::Bottom:
            penPos.y += destRect.h() - linesHeight;
            break;
        }

        // move down to the bottom of the line, minus the descent value (so the descent of the glyphs dont extend past the whole line)
        penPos.y += singleLineHeight + singleLineDescent;

        // render lines one by one
        for (auto& line : lines) {
            auto const shapeInfo = vulkanFont.ShapeString(line.text);

            switch (horizontalAlignment) {
            case TextHorizAlignment::Left:
                penPos.x = static_cast<float>(destRect.topLeft.x);
                break;
            case TextHorizAlignment::Center:
                penPos.x = static_cast<float>(destRect.topLeft.x) + ((destRect.w() - line.lineWidth) / 2.0f);
                break;
            case TextHorizAlignment::Right:
                penPos.x = static_cast<float>(destRect.bottomRight.x) - line.lineWidth;
                break;
            }

            for (auto const& info : shapeInfo) {
                if (info.glyphIndex >= 0) {
                    auto const bearing = static_cast<FloatVec2>(vulkanFont.GetGlyphBearing(info.glyphIndex));
                    auto const offset = static_cast<FloatVec2>(info.offset);
                    auto const glyphPos = penPos + bearing + offset;
                    SubmitCharacter(info.glyphIndex, glyphPos);
                }
                penPos.x += info.advance.x;
            }

            penPos.y += singleLineHeight;
        }

        context->m_fontInstanceStagingBuffer->Unmap();

        uint32_t const glyphCount = context->m_glyphCount - glyphStart;
        if (glyphCount) {
            auto& commandBuffer = context->m_target->GetCommandBuffer();

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
        auto context = m_contextStack.top();
        auto& commandBuffer = context->m_target->GetCommandBuffer();
        if (clipRect) {
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

        if (target) {
            m_overrideContext.m_target = dynamic_cast<Framebuffer*>(target);
            assert(m_overrideContext.m_target);
            VkFence fence = m_overrideContext.m_target->GetFence().GetVkFence();
            vkResetFences(m_surfaceContext.GetVkDevice(), 1, &fence);
            BeginContext(&m_overrideContext);
        }
    }

    void Graphics::SetLogicalSize(IntVec2 const& logicalSize) {
        auto context = m_contextStack.top();
        auto& commandBuffer = context->m_target->GetCommandBuffer();
        PushConstants constants;
        constants.xyScale = { 2.0f / static_cast<float>(logicalSize.x), 2.0f / static_cast<float>(logicalSize.y) };
        constants.xyOffset = { -1.0f, -1.0f };
        commandBuffer.PushConstants(*m_drawingShader, VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstants), &constants);
        commandBuffer.PushConstants(*m_fontShader, VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstants), &constants);
    }

    VkDescriptorSet Graphics::GetDescriptorSet(Texture& image) {
        return m_drawingShader->GetDescriptorSet(image);
    }

    VkPrimitiveTopology Graphics::ToVulkan(ETopologyType type) const {
        switch (type) {
        default:
            assert(false);
        case ETopologyType::Lines:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case ETopologyType::Triangles:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        }
    }

    VkPipelineColorBlendAttachmentState Graphics::ToVulkan(BlendMode mode) const {
        VkPipelineColorBlendAttachmentState currentBlend{};
        switch (mode) {
        default:
        case BlendMode::Replace:
            currentBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
            currentBlend.blendEnable = VK_FALSE;
            currentBlend.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            currentBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            currentBlend.colorBlendOp = VK_BLEND_OP_ADD;
            currentBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            currentBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            currentBlend.alphaBlendOp = VK_BLEND_OP_ADD;
            break;
        case BlendMode::Add:
            currentBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            currentBlend.blendEnable = VK_TRUE;
            currentBlend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            currentBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
            currentBlend.colorBlendOp = VK_BLEND_OP_ADD;
            currentBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            currentBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            currentBlend.alphaBlendOp = VK_BLEND_OP_ADD;
            break;
        case BlendMode::Alpha:
            currentBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            currentBlend.blendEnable = VK_TRUE;
            currentBlend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            currentBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            currentBlend.colorBlendOp = VK_BLEND_OP_ADD;
            currentBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            currentBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            currentBlend.alphaBlendOp = VK_BLEND_OP_ADD;
            break;
        case BlendMode::Modulate:
            currentBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            currentBlend.blendEnable = VK_TRUE;
            currentBlend.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            currentBlend.dstColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
            currentBlend.colorBlendOp = VK_BLEND_OP_ADD;
            currentBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            currentBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            currentBlend.alphaBlendOp = VK_BLEND_OP_ADD;
            break;
        case BlendMode::Multiply:
            currentBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            currentBlend.blendEnable = VK_TRUE;
            currentBlend.srcColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
            currentBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            currentBlend.colorBlendOp = VK_BLEND_OP_ADD;
            currentBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
            currentBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            currentBlend.alphaBlendOp = VK_BLEND_OP_ADD;
            break;
        }
        return currentBlend;
    }

    void Graphics::CreateRenderPass() {
        {
            VkAttachmentDescription colorAttachment{};
            // colorAttachment.format = VK_FORMAT_R8G8B8A8_SRGB; // TODO this might have to change?
            colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorAttachmentRef{};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;

            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            m_renderPass = RenderPassBuilder(m_surfaceContext.GetVkDevice())
                               .AddAttachment(colorAttachment)
                               .AddSubpass(subpass)
                               .AddDependency(dependency)
                               .Build();
        }
        {
            // specifically for rendering to render targets
            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            VkAttachmentReference colorAttachmentRef{};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;

            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            m_rtRenderPass = RenderPassBuilder(m_surfaceContext.GetVkDevice())
                                 .AddAttachment(colorAttachment)
                                 .AddSubpass(subpass)
                                 .AddDependency(dependency)
                                 .Build();
        }
    }

    void Graphics::CreateShaders() {
        m_drawingShader = ShaderBuilder(m_surfaceContext.GetVkDevice(), m_surfaceContext.GetVkDescriptorPool())
                              .AddPushConstant(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants))
                              .AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT)
                              .AddStage(VK_SHADER_STAGE_VERTEX_BIT, "main", drawing_shader_vert_spv, drawing_shader_vert_spv_len)
                              .AddStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main", drawing_shader_frag_spv, drawing_shader_frag_spv_len)
                              .Build();

        m_fontShader = ShaderBuilder(m_surfaceContext.GetVkDevice(), m_surfaceContext.GetVkDescriptorPool())
                           .AddPushConstant(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants))
                           .AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT)
                           .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT)
                           .AddStage(VK_SHADER_STAGE_VERTEX_BIT, "main", font_vert_spv, font_vert_spv_len)
                           .AddStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main", font_frag_spv, font_frag_spv_len)
                           .Build();
    }

    void Graphics::CreateDefaultImage() {
        auto stagingBuffer = std::make_unique<Buffer>(m_surfaceContext, 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        unsigned char const pixel[] = { 0xFF, 0xFF, 0xFF, 0xFF };
        void* data = stagingBuffer->Map();
        memcpy(data, pixel, 4);
        stagingBuffer->Unmap();

        const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

        m_defaultImage = std::make_unique<Texture>(m_surfaceContext, 1, 1, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

        auto commandBuffer = std::make_unique<CommandBuffer>(m_surfaceContext);
        commandBuffer->BeginRecord();
        commandBuffer->TransitionImageLayout(*m_defaultImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        commandBuffer->CopyBufferToImage(*m_defaultImage, *stagingBuffer);
        commandBuffer->TransitionImageLayout(*m_defaultImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        commandBuffer->SubmitAndWait();
    }

    RenderPass& Graphics::GetCurrentRenderPass() {
        return IsRenderTarget() ? *m_rtRenderPass : *m_renderPass;
    }

    Pipeline& Graphics::GetCurrentPipeline(ETopologyType topology) {
        auto context = m_contextStack.top();
        auto const vkTopology = ToVulkan(topology);
        auto const blendAttachment = ToVulkan(context->m_currentBlendMode);
        auto const vertexInputBinding = getVertexBindingDescription();
        auto const vertexAttributeBindings = getVertexAttributeDescriptions();

        auto const builder = PipelineBuilder(m_surfaceContext.GetVkDevice())
                                 .SetPipelineCache(m_vkPipelineCache)
                                 .SetRenderPass(GetCurrentRenderPass())
                                 .SetShader(m_drawingShader)
                                 .AddVertexInputBinding(vertexInputBinding)
                                 .AddVertexAttribute(vertexAttributeBindings[0])
                                 .AddVertexAttribute(vertexAttributeBindings[1])
                                 .AddVertexAttribute(vertexAttributeBindings[2])
                                 .AddColorBlendAttachment(blendAttachment)
                                 .AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT)
                                 .AddDynamicState(VK_DYNAMIC_STATE_SCISSOR)
                                 .SetTopology(vkTopology);

        uint32_t const pipelineHash = builder.CalculateHash();
        auto it = m_pipelines.find(pipelineHash);
        if (std::end(m_pipelines) == it) {
            auto const insertResult = m_pipelines.insert(std::make_pair(pipelineHash, builder.Build()));
            it = insertResult.first;
        }

        return *it->second;
    }

    Pipeline& Graphics::GetCurrentFontPipeline() {
        auto context = m_contextStack.top();
        auto const vkTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        auto const blendAttachment = ToVulkan(context->m_currentBlendMode);
        auto const vertexInputBinding = getFontVertexBindingDescription();
        auto const vertexAttributeBindings = getFontVertexAttributeDescriptions();

        auto const builder = PipelineBuilder(m_surfaceContext.GetVkDevice())
                                 .SetPipelineCache(m_vkPipelineCache)
                                 .SetRenderPass(GetCurrentRenderPass())
                                 .SetShader(m_fontShader)
                                 .AddVertexInputBinding(vertexInputBinding)
                                 .AddVertexAttribute(vertexAttributeBindings[0])
                                 .AddVertexAttribute(vertexAttributeBindings[1])
                                 .AddVertexAttribute(vertexAttributeBindings[2])
                                 .AddColorBlendAttachment(blendAttachment)
                                 .AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT)
                                 .AddDynamicState(VK_DYNAMIC_STATE_SCISSOR)
                                 .SetTopology(vkTopology);

        uint32_t const pipelineHash = builder.CalculateHash();
        auto it = m_fontPipelines.find(pipelineHash);
        if (std::end(m_fontPipelines) == it) {
            auto const insertResult = m_fontPipelines.insert(std::make_pair(pipelineHash, builder.Build()));
            it = insertResult.first;
        }

        return *it->second;
    }

    void Graphics::BeginContext(DrawContext* context) {
        context->m_logicalExtent = context->m_target->GetVkExtent();
        context->m_vertexCount = 0;
        context->m_maxVertexCount = 1024;
        context->m_currentPipelineId = 0;

        if (!context->m_vertexBuffer) {
            context->m_vertexBuffer = std::make_unique<Buffer>(m_surfaceContext, 1024 * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            context->m_vertexBufferData = static_cast<Vertex*>(context->m_vertexBuffer->Map());
        }

        if (!context->m_fontInstanceBuffer) {
            context->m_fontInstanceBuffer = std::make_unique<Buffer>(m_surfaceContext, 1024 * sizeof(FontGlyphInstance), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            context->m_fontInstanceStagingBuffer = std::make_unique<Buffer>(m_surfaceContext, 1024 * sizeof(FontGlyphInstance), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        }

        context->m_glyphCount = 0;

        m_contextStack.push(context);
        StartCommands();
    }

    void Graphics::EndContext() {
        FlushCommands();
        m_contextStack.pop();
    }

    void Graphics::StartCommands() {
        auto context = m_contextStack.top();
        auto& commandBuffer = context->m_target->GetCommandBuffer();
        commandBuffer.Reset();
        commandBuffer.BeginRecord();
        commandBuffer.BeginRenderPass(GetCurrentRenderPass(), *context->m_target);

        VkViewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = static_cast<float>(context->m_logicalExtent.width);
        viewport.height = static_cast<float>(context->m_logicalExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        commandBuffer.SetViewport(viewport);

        VkRect2D scissor;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = context->m_target->GetVkExtent();
        commandBuffer.SetScissor(scissor);

        PushConstants pushConstants;
        pushConstants.xyScale = { 2.0f / static_cast<float>(context->m_logicalExtent.width), 2.0f / static_cast<float>(context->m_logicalExtent.height) };
        pushConstants.xyOffset = { -1.0f, -1.0f };
        commandBuffer.PushConstants(*m_drawingShader, VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstants), &pushConstants);
    }

    void Graphics::FlushCommands() {
        auto context = m_contextStack.top();
        VkFence cmdFence = context->m_target->GetFence().GetVkFence();
        auto& commandBuffer = context->m_target->GetCommandBuffer();
        commandBuffer.EndRenderPass();
        commandBuffer.EndRecord();

        if (context->m_glyphCount) {
            auto uploadCommandBuffer = std::make_unique<CommandBuffer>(m_surfaceContext);
            uploadCommandBuffer->BeginRecord();

            VkBufferMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            barrier.srcAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.buffer = context->m_fontInstanceStagingBuffer->GetVKBuffer();
            barrier.offset = 0;
            barrier.size = context->m_fontInstanceStagingBuffer->GetSize();

            vkCmdPipelineBarrier(uploadCommandBuffer->GetVkCommandBuffer(),
                                 VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 0, 0, nullptr, 1, &barrier, 0, nullptr);

            VkBufferCopy copy;
            copy.srcOffset = 0;
            copy.dstOffset = 0;
            copy.size = context->m_fontInstanceStagingBuffer->GetSize();

            vkCmdCopyBuffer(uploadCommandBuffer->GetVkCommandBuffer(), context->m_fontInstanceStagingBuffer->GetVKBuffer(), context->m_fontInstanceBuffer->GetVKBuffer(), 1, &copy);

            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;

            vkCmdPipelineBarrier(uploadCommandBuffer->GetVkCommandBuffer(),
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                                 0, 0, nullptr, 1, &barrier, 0, nullptr);

            uploadCommandBuffer->SubmitAndWait();
        }

        commandBuffer.Submit(cmdFence, context->m_target->GetAvailableSemaphore(), context->m_target->GetRenderFinishedSemaphore());
        vkWaitForFences(m_surfaceContext.GetVkDevice(), 1, &cmdFence, VK_TRUE, UINT64_MAX);
    }

    void Graphics::SubmitVertices(Vertex* vertices, uint32_t vertCount, ETopologyType topology, VkDescriptorSet descriptorSet) {
        auto context = m_contextStack.top();

        assert(vertCount <= context->m_maxVertexCount);

        const uint32_t availableVertices = context->m_maxVertexCount - context->m_vertexCount;
        if (availableVertices < vertCount) {
            FlushCommands();
            StartCommands();
        }

        const uint32_t vertexDataSize = sizeof(Vertex) * vertCount;
        const uint32_t existingVertexOffset = /*sizeof(Vertex) **/ context->m_vertexCount;
        memcpy(context->m_vertexBufferData + existingVertexOffset, vertices, vertexDataSize);

        auto& commandBuffer = context->m_target->GetCommandBuffer();
        commandBuffer.BindVertexBuffer(*context->m_vertexBuffer, 0);

        auto const& pipeline = GetCurrentPipeline(topology);
        if (context->m_currentPipelineId != pipeline.m_hash) {
            commandBuffer.BindPipeline(pipeline);
            context->m_currentPipelineId = pipeline.m_hash;
        }

        if (descriptorSet != VK_NULL_HANDLE) {
            commandBuffer.BindDescriptorSet(*m_drawingShader, descriptorSet, 0);
        } else {
            VkDescriptorSet defaultDescriptorSet = m_drawingShader->GetDescriptorSet(*m_defaultImage);
            commandBuffer.BindDescriptorSet(*m_drawingShader, defaultDescriptorSet, 0);
        }

        commandBuffer.Draw(vertCount, context->m_vertexCount);
        context->m_vertexCount += vertCount;
    }

    void Graphics::OnResize(VkSurfaceKHR surface, uint32_t surfaceWidth, uint32_t surfaceHeight) {
        vkDeviceWaitIdle(m_surfaceContext.GetVkDevice());
        m_swapchain.reset();
        m_swapchain = std::make_unique<Swapchain>(m_surfaceContext, *m_renderPass, surface, VkExtent2D{ surfaceWidth, surfaceHeight });
    }
}

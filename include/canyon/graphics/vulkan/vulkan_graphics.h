#pragma once

#include "canyon/graphics/blend_mode.h"
#include "canyon/graphics/color.h"
#include "canyon/graphics/ifont.h"
#include "canyon/graphics/igraphics.h"
#include "canyon/graphics/iimage.h"
#include "canyon/graphics/itarget.h"
#include "canyon/graphics/text_alignment.h"
#include "canyon/graphics/vulkan/vulkan_buffer.h"
#include "canyon/graphics/vulkan/vulkan_command_buffer.h"
#include "canyon/graphics/vulkan/vulkan_image.h"
#include "canyon/graphics/vulkan/vulkan_pipeline.h"
#include "canyon/graphics/vulkan/vulkan_renderpass.h"
#include "canyon/graphics/vulkan/vulkan_shader.h"
#include "canyon/graphics/vulkan/vulkan_surface_context.h"
#include "canyon/graphics/vulkan/vulkan_swapchain.h"
#include "canyon/graphics/vulkan/vulkan_texture.h"
#include "canyon/platform/window.h"
#include "canyon/utils/rect.h"
#include "canyon/utils/vector.h"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <stack>
#include <string>

namespace canyon::graphics::vulkan {
    class Graphics : public IGraphics {
    public:
        Graphics(SurfaceContext& context, VkSurfaceKHR surface, uint32_t surfaceWidth, uint32_t surfaceHeight);
        ~Graphics();

        void InitImgui(canyon::platform::Window const& window) override;

        SurfaceContext& GetContext() const override { return m_surfaceContext; }

        struct Vertex {
            FloatVec2 xy;
            FloatVec2 uv;
            Color color;
        };

        struct FontRect {
            float min_x;
            float min_y;
            float max_x;
            float max_y;
        };

        struct FontGlyphInstance {
            FloatVec2 pos;
            uint32_t glyphIndex;
            Color color;
        };

        void Begin() override;
        void End() override;

        void SetBlendMode(BlendMode mode) override;
        // void SetBlendMode(std::shared_ptr<IImage> target, EBlendMode mode) override;
        // void SetColorMod(std::shared_ptr<IImage> target, Color const& color) override;
        void SetColor(Color const& color) override;
        void Clear() override;
        void DrawImage(IImage& image, IntRect const& destRect, IntRect const* sourceRect) override;
        void DrawImageTiled(graphics::IImage& image, IntRect const& destRect, IntRect const* sourceRect, float scale) override;
        void DrawToPNG(std::filesystem::path const& path) override;
        void DrawRectF(FloatRect const& rect) override;
        void DrawFillRectF(FloatRect const& rect) override;
        void DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) override;
        void DrawText(std::string const& text, IFont& font, TextHorizAlignment horizontalAlignment, TextVertAlignment verticalAlignment, IntRect const& destRect) override;
        void SetClip(IntRect const* clipRect) override;

        std::unique_ptr<ITarget> CreateTarget(int width, int height) override;
        ITarget* GetTarget() override;
        void SetTarget(ITarget* target) override;

        void SetLogicalSize(IntVec2 const& logicalSize) override;

        Swapchain& GetSwapchain() const { return *m_swapchain; }
        RenderPass& GetRenderPass() const { return *m_renderPass; }
        CommandBuffer* GetCurrentCommandBuffer() {
            auto context = m_contextStack.top();
            if (context) {
                return &context->m_target->GetCommandBuffer();
            }
            return nullptr;
        }
        VkDescriptorSet GetDescriptorSet(Texture& image);

        Shader& GetFontShader() { return *m_fontShader; }

        void OnResize(VkSurfaceKHR surface, uint32_t surfaceWidth, uint32_t surfaceHeight);

    private:
        bool m_imguiInitialized = false;
        SurfaceContext& m_surfaceContext;

        struct PushConstants {
            FloatVec2 xyScale;
            FloatVec2 xyOffset;
        };

        enum class ETopologyType {
            Invalid,
            Lines,
            Triangles
        };

        struct DrawContext {
            Framebuffer* m_target = nullptr;
            VkExtent2D m_logicalExtent;

            BlendMode m_currentBlendMode = BlendMode::Replace;
            Color m_currentColor = BasicColors::White;

            std::unique_ptr<Buffer> m_vertexBuffer;
            Vertex* m_vertexBufferData = nullptr;

            std::unique_ptr<Buffer> m_fontInstanceBuffer;
            std::unique_ptr<Buffer> m_fontInstanceStagingBuffer;
            uint32_t m_glyphCount = 0;

            uint32_t m_vertexCount = 0;
            uint32_t m_maxVertexCount = 0;
            uint32_t m_currentPipelineId = 0;
        };

        VkPipelineCache m_vkPipelineCache;
        std::map<uint32_t, std::shared_ptr<Pipeline>> m_pipelines;
        std::map<uint32_t, std::shared_ptr<Pipeline>> m_fontPipelines;
        std::unique_ptr<RenderPass> m_renderPass;
        std::unique_ptr<RenderPass> m_rtRenderPass;
        std::unique_ptr<Swapchain> m_swapchain;
        std::shared_ptr<Shader> m_drawingShader;
        std::shared_ptr<Shader> m_fontShader;
        std::unique_ptr<Texture> m_defaultImage;

        DrawContext m_defaultContext;
        DrawContext m_overrideContext;
        std::stack<DrawContext*> m_contextStack;

        VkPrimitiveTopology ToVulkan(ETopologyType type) const;
        VkPipelineColorBlendAttachmentState ToVulkan(BlendMode mode) const;

        void CreateRenderPass();
        void CreateShaders();
        void CreateDefaultImage();
        RenderPass& GetCurrentRenderPass();
        Pipeline& GetCurrentPipeline(ETopologyType topology);
        Pipeline& GetCurrentFontPipeline();

        void BeginContext(DrawContext* target);
        void EndContext();
        void StartCommands();
        void FlushCommands();
        void SubmitVertices(Vertex* vertices, uint32_t vertCount, ETopologyType topology, VkDescriptorSet descriptorSet = VK_NULL_HANDLE);

        bool IsRenderTarget() const;
    };
}

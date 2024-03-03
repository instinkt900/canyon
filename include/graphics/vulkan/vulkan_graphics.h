#pragma once

#include "graphics/igraphics_context.h"
#include "vulkan_image.h"
#include "vulkan_context.h"
#include "vulkan_utils.h"
#include "vulkan_shader.h"
#include "vulkan_pipeline.h"
#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_swapchain.h"

#include <stack>

namespace graphics::vulkan {
    class Graphics : public IGraphicsContext {
    public:
        Graphics(Context& context, VkSurfaceKHR surface, uint32_t surfaceWidth, uint32_t surfaceHeight);
        ~Graphics();

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

        void Begin();
        void End();

        void SetBlendMode(BlendMode mode) override;
        //void SetBlendMode(std::shared_ptr<IImage> target, EBlendMode mode) override;
        //void SetColorMod(std::shared_ptr<IImage> target, Color const& color) override;
        void SetColor(Color const& color) override;
        void Clear() override;
        void DrawImage(IImage& image, IntRect const* sourceRect, IntRect const* destRect) override;
        void DrawToPNG(std::filesystem::path const& path) override;
        void DrawRectF(FloatRect const& rect) override;
        void DrawFillRectF(FloatRect const& rect) override;
        void DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) override;
        void DrawText(std::string const& text, IFont& font, TextHorizAlignment horizontalAlignment, TextVertAlignment verticalAlignment, IntRect const& destRect) override;
        void SetClipRect(IntRect const* clipRect);

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
        VkDescriptorSet GetDescriptorSet(Image& image);

        Shader& GetFontShader() { return *m_fontShader; }

        void OnResize(VkSurfaceKHR surface, uint32_t surfaceWidth, uint32_t surfaceHeight);
    private:
        Context& m_context;

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
        std::unique_ptr<Image> m_defaultImage;

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

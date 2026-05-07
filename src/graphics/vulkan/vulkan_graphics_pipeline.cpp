#include "common.h"
#include "vulkan_graphics.h"
#include <stdexcept>
#include "vulkan_command_buffer.h"
#include "vulkan_utils.h"
#include "shaders/vulkan_shaders.h"

namespace {
    VkVertexInputBindingDescription getVertexBindingDescription() {
        VkVertexInputBindingDescription vertexBindingDesc{};

        vertexBindingDesc.binding = 0;
        vertexBindingDesc.stride = sizeof(moth_graphics::graphics::vulkan::Graphics::Vertex);
        vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return vertexBindingDesc;
    }

    std::vector<VkVertexInputAttributeDescription> getVertexAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescs(3);

        vertexAttributeDescs[0].binding = 0;
        vertexAttributeDescs[0].location = 0;
        vertexAttributeDescs[0].format = VK_FORMAT_R32G32_SFLOAT;
        vertexAttributeDescs[0].offset = offsetof(moth_graphics::graphics::vulkan::Graphics::Vertex, xy);

        vertexAttributeDescs[1].binding = 0;
        vertexAttributeDescs[1].location = 1;
        vertexAttributeDescs[1].format = VK_FORMAT_R32G32_SFLOAT;
        vertexAttributeDescs[1].offset = offsetof(moth_graphics::graphics::vulkan::Graphics::Vertex, uv);

        vertexAttributeDescs[2].binding = 0;
        vertexAttributeDescs[2].location = 2;
        vertexAttributeDescs[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        vertexAttributeDescs[2].offset = offsetof(moth_graphics::graphics::vulkan::Graphics::Vertex, color);

        return vertexAttributeDescs;
    }

    VkVertexInputBindingDescription getFontVertexBindingDescription() {
        VkVertexInputBindingDescription vertexBindingDesc{};

        vertexBindingDesc.binding = 0;
        vertexBindingDesc.stride = sizeof(moth_graphics::graphics::vulkan::Graphics::FontGlyphInstance);
        vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        return vertexBindingDesc;
    }

    std::vector<VkVertexInputAttributeDescription> getFontVertexAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescs(4);

        vertexAttributeDescs[0].binding = 0;
        vertexAttributeDescs[0].location = 0;
        vertexAttributeDescs[0].format = VK_FORMAT_R32G32_SFLOAT;
        vertexAttributeDescs[0].offset = offsetof(moth_graphics::graphics::vulkan::Graphics::FontGlyphInstance, pos);

        vertexAttributeDescs[1].binding = 0;
        vertexAttributeDescs[1].location = 1;
        vertexAttributeDescs[1].format = VK_FORMAT_R32_UINT;
        vertexAttributeDescs[1].offset = offsetof(moth_graphics::graphics::vulkan::Graphics::FontGlyphInstance, glyphIndex);

        vertexAttributeDescs[2].binding = 0;
        vertexAttributeDescs[2].location = 2;
        vertexAttributeDescs[2].format = VK_FORMAT_R32_SFLOAT;
        vertexAttributeDescs[2].offset = offsetof(moth_graphics::graphics::vulkan::Graphics::FontGlyphInstance, rotation);

        vertexAttributeDescs[3].binding = 0;
        vertexAttributeDescs[3].location = 3;
        vertexAttributeDescs[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        vertexAttributeDescs[3].offset = offsetof(moth_graphics::graphics::vulkan::Graphics::FontGlyphInstance, color);

        return vertexAttributeDescs;
    }
}

namespace moth_graphics::graphics::vulkan {
    VkDescriptorSet Graphics::GetDescriptorSet(Texture& image) {
        return m_drawingShader->GetDescriptorSet(image);
    }

    VkPrimitiveTopology Graphics::ToVulkan(ETopologyType type) {
        switch (type) {
        case ETopologyType::Lines:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case ETopologyType::Triangles:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        default:
            assert(false && "Unknown ETopologyType");
            throw std::runtime_error("Unknown ETopologyType");
        }
    }

    VkPipelineColorBlendAttachmentState Graphics::ToVulkan(BlendMode mode) {
        VkPipelineColorBlendAttachmentState currentBlend{};
        switch (mode) {
        default:
        case BlendMode::Replace:
            currentBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
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
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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
        auto* context = CurrentContext();
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
        auto* context = CurrentContext();
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
                                 .AddVertexAttribute(vertexAttributeBindings[3])
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
}

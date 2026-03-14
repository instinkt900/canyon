#include "common.h"
#include "canyon/graphics/vulkan/vulkan_graphics.h"
#include "canyon/graphics/vulkan/vulkan_command_buffer.h"
#include "canyon/graphics/vulkan/vulkan_utils.h"

namespace canyon::graphics::vulkan {
    void Graphics::BeginContext(DrawContext* context) {
        context->m_logicalExtent = context->m_target->GetVkExtent();
        context->m_vertexCount = 0;
        context->m_maxVertexCount = 1024;
        context->m_currentPipelineId = 0;
        context->m_pendingBatch.reset();

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

    void Graphics::RestartContext() {
        auto* context = m_contextStack.top();
        FlushCommands();
        auto* cmdFence = context->m_target->GetFence().GetVkFence();
        vkWaitForFences(m_surfaceContext.GetVkDevice(), 1, &cmdFence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_surfaceContext.GetVkDevice(), 1, &cmdFence);
        context->m_logicalExtent = context->m_target->GetVkExtent();
        context->m_vertexCount = 0;
        context->m_maxVertexCount = 1024;
        context->m_currentPipelineId = 0;
        context->m_pendingBatch.reset();
        context->m_glyphCount = 0;
        StartCommands();
    }

    void Graphics::EndContext() {
        FlushCommands();
        m_contextStack.pop();
    }

    void Graphics::StartCommands() {
        auto* context = m_contextStack.top();
        auto& commandBuffer = context->m_target->GetCommandBuffer();
        commandBuffer.Reset();
        commandBuffer.BeginRecord();
        commandBuffer.HostWriteToVertexBarrier(*context->m_vertexBuffer);
        if (IsRenderTarget()) {
            commandBuffer.TransitionImageLayout(*context->m_target->GetVkImage().m_texture, context->m_target->GetVkFormat(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        }
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
        commandBuffer.BindVertexBuffer(*context->m_vertexBuffer, 0);
    }

    void Graphics::FlushPendingBatch() {
        auto* context = CurrentContext();
        if (!context->m_pendingBatch) {
            return;
        }
        auto const& batch = *context->m_pendingBatch;
        auto& commandBuffer = context->m_target->GetCommandBuffer();
        commandBuffer.BindVertexBuffer(*context->m_vertexBuffer, 0);
        commandBuffer.BindDescriptorSet(*m_drawingShader, batch.m_descriptorSet, 0);
        commandBuffer.Draw(batch.m_vertexCount, batch.m_firstVertex);
        context->m_pendingBatch.reset();
    }

    void Graphics::FlushCommands() {
        FlushPendingBatch();
        auto* context = CurrentContext();
        VkFence cmdFence = context->m_target->GetFence().GetVkFence();
        auto& commandBuffer = context->m_target->GetCommandBuffer();
        commandBuffer.EndRenderPass();
        commandBuffer.EndRecord();

        if (context->m_glyphCount != 0u) {
            auto uploadCommandBuffer = std::make_unique<CommandBuffer>(m_surfaceContext);
            uploadCommandBuffer->BeginRecord();

            VkBufferMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.buffer = context->m_fontInstanceStagingBuffer->GetVKBuffer();
            barrier.offset = 0;
            barrier.size = context->m_fontInstanceStagingBuffer->GetSize();

            vkCmdPipelineBarrier(uploadCommandBuffer->GetVkCommandBuffer(),
                                 VK_PIPELINE_STAGE_HOST_BIT,
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
    }

    void Graphics::SubmitVertices(Vertex* vertices, uint32_t vertCount, ETopologyType topology, VkDescriptorSet descriptorSet) {
        auto* context = CurrentContext();

        assert(vertCount <= context->m_maxVertexCount);

        const uint32_t availableVertices = context->m_maxVertexCount - context->m_vertexCount;
        if (availableVertices < vertCount) {
            RestartContext();
        }

        const uint32_t existingVertexOffset = context->m_vertexCount;
        memcpy(context->m_vertexBufferData + existingVertexOffset, vertices, sizeof(Vertex) * vertCount);

        auto& commandBuffer = context->m_target->GetCommandBuffer();

        auto const& pipeline = GetCurrentPipeline(topology);
        if (context->m_currentPipelineId != pipeline.m_hash) {
            FlushPendingBatch();
            commandBuffer.BindPipeline(pipeline);
            context->m_currentPipelineId = pipeline.m_hash;
        }

        VkDescriptorSet resolvedDescriptorSet = (descriptorSet != VK_NULL_HANDLE)
            ? descriptorSet
            : m_drawingShader->GetDescriptorSet(*m_defaultImage);

        if (context->m_pendingBatch && context->m_pendingBatch->m_descriptorSet == resolvedDescriptorSet) {
            context->m_pendingBatch->m_vertexCount += vertCount;
        } else {
            FlushPendingBatch();
            context->m_pendingBatch = DrawContext::PendingBatch{ existingVertexOffset, vertCount, resolvedDescriptorSet };
        }

        context->m_vertexCount += vertCount;
    }
}

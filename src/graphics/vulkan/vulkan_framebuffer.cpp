#include "common.h"
#include "vulkan_framebuffer.h"
#include "vulkan_utils.h"
#include "vulkan_command_buffer.h"
#include "vulkan_swapchain.h"
#include <vulkan/vulkan_core.h>

namespace moth_graphics::graphics::vulkan {
    Framebuffer::Framebuffer(SurfaceContext& context, uint32_t width, uint32_t height, VkImage image, VkImageView view, VkFormat format, VkRenderPass renderPass, uint32_t swapchainIndex)
        : m_context(context)
        , m_swapchainIndex(swapchainIndex) {
        m_commandBuffer = std::make_unique<CommandBuffer>(context);
        m_fence = std::make_unique<Fence>(context);

        m_texture = std::make_shared<Texture>(context, image, view, VkExtent2D{ width, height }, format, false);
        CreateFramebufferResource(renderPass);
    }

    Framebuffer::Framebuffer(SurfaceContext& context, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkRenderPass renderPass)
        : m_context(context) {
        m_commandBuffer = std::make_unique<CommandBuffer>(context);
        m_fence = std::make_unique<Fence>(context);

        m_texture = std::make_shared<Texture>(context, width, height, format, tiling, usage);
        CreateFramebufferResource(renderPass);

        m_commandBuffer->BeginRecord();
        m_commandBuffer->TransitionImageLayout(*m_texture, m_texture->GetVkFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_commandBuffer->SubmitAndWait();
    }

    Framebuffer::~Framebuffer() {
        vkDestroyFramebuffer(m_context.GetVkDevice(), m_vkFramebuffer, nullptr);
    }

    VkExtent2D Framebuffer::GetVkExtent() const {
        return m_texture->GetVkExtent();
    }

    VkFormat Framebuffer::GetVkFormat() const {
        return m_texture->GetVkFormat();
    }

    VkSemaphore Framebuffer::GetAvailableSemaphore() const {
        return m_frameSlot ? m_frameSlot->imageAvailable : VK_NULL_HANDLE;
    }
    VkSemaphore Framebuffer::GetRenderFinishedSemaphore() const {
        return m_frameSlot ? m_frameSlot->renderFinished : VK_NULL_HANDLE;
    }

    void Framebuffer::CreateFramebufferResource(VkRenderPass renderPass) {
        VkImageView attachments[] = { m_texture->GetVkView() };
        VkFramebufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.renderPass = renderPass;
        info.pAttachments = attachments;
        info.attachmentCount = 1;
        info.width = m_texture->GetWidth();
        info.height = m_texture->GetHeight();
        info.layers = 1;
        CHECK_VK_RESULT(vkCreateFramebuffer(m_context.GetVkDevice(), &info, nullptr, &m_vkFramebuffer));
    }
}

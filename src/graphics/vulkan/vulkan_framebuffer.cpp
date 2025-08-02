#include "common.h"
#include "canyon/graphics/vulkan/vulkan_framebuffer.h"
#include "canyon/graphics/vulkan/vulkan_utils.h"
#include "canyon/graphics/vulkan/vulkan_command_buffer.h"
#include <vulkan/vulkan_core.h>

namespace canyon::graphics::vulkan {
    Framebuffer::Framebuffer(SurfaceContext& context, uint32_t width, uint32_t height, VkImage image, VkImageView view, VkFormat format, VkRenderPass renderPass, uint32_t swapchainIndex)
        : m_context(context)
        , m_swapchainIndex(swapchainIndex) {
        m_commandBuffer = std::make_unique<CommandBuffer>(context);
        m_fence = std::make_unique<Fence>(context);

        auto texture = std::make_unique<Texture>(context, image, view, VkExtent2D{ width, height }, format, false);
        IntRect rec = { { 0, 0 }, { width, height } };
        m_image = std::make_unique<Image>(std::move(texture), rec);
        CreateFramebufferResource(renderPass);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        CHECK_VK_RESULT(vkCreateSemaphore(m_context.GetVkDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphore));
        CHECK_VK_RESULT(vkCreateSemaphore(m_context.GetVkDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphore));
    }

    Framebuffer::Framebuffer(SurfaceContext& context, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkRenderPass renderPass)
        : m_context(context) {
        m_commandBuffer = std::make_unique<CommandBuffer>(context);
        m_fence = std::make_unique<Fence>(context);

        auto texture = std::make_unique<Texture>(context, width, height, format, tiling, usage);
        IntRect rec = { { 0, 0 }, { width, height } };
        m_image = std::make_unique<Image>(std::move(texture), rec);
        CreateFramebufferResource(renderPass);

        m_commandBuffer->BeginRecord();
        m_commandBuffer->TransitionImageLayout(*m_image->m_texture, m_image->m_texture->GetVkFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_commandBuffer->SubmitAndWait();
    }

    Framebuffer::~Framebuffer() {
        if (m_imageAvailableSemaphore) {
            vkDestroySemaphore(m_context.GetVkDevice(), m_imageAvailableSemaphore, nullptr);
        }
        if (m_renderFinishedSemaphore) {
            vkDestroySemaphore(m_context.GetVkDevice(), m_renderFinishedSemaphore, nullptr);
        }
        vkDestroyFramebuffer(m_context.GetVkDevice(), m_vkFramebuffer, nullptr);
    }

    VkExtent2D Framebuffer::GetVkExtent() const {
        return m_image->m_texture->GetVkExtent();
    }

    VkFormat Framebuffer::GetVkFormat() const {
        return m_image->m_texture->GetVkFormat();
    }

    Image& Framebuffer::GetVkImage() {
        return *m_image;
    }

    void Framebuffer::CreateFramebufferResource(VkRenderPass renderPass) {
        VkImageView attachments[] = { m_image->m_texture->GetVkView() };
        VkFramebufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.renderPass = renderPass;
        info.pAttachments = attachments;
        info.attachmentCount = 1;
        info.width = m_image->GetWidth();
        info.height = m_image->GetHeight();
        info.layers = 1;
        CHECK_VK_RESULT(vkCreateFramebuffer(m_context.GetVkDevice(), &info, nullptr, &m_vkFramebuffer));
    }
}

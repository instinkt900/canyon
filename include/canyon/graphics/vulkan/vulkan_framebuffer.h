#pragma once

#include "canyon/graphics/iimage.h"
#include "canyon/graphics/itarget.h"
#include "canyon/graphics/vulkan/vulkan_fence.h"
#include "canyon/graphics/vulkan/vulkan_image.h"
#include "canyon/graphics/vulkan/vulkan_surface_context.h"

#include <vulkan/vulkan_core.h>

#include <memory>
#include <cstdint>

namespace canyon::graphics::vulkan {
    class CommandBuffer;

    class Framebuffer : public ITarget {
    public:
        Framebuffer(SurfaceContext& context, uint32_t width, uint32_t height, VkImage image, VkImageView view, VkFormat format, VkRenderPass renderPass, uint32_t swapchainIndex);
        Framebuffer(SurfaceContext& context, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkRenderPass renderPass);
        virtual ~Framebuffer();

        // void BeginPass(RenderPass& renderPass);
        // void EndPass();
        // void Submit();
        
        int GetWidth() const { return m_image ? m_image->GetWidth() : 0; }
        int GetHeight() const { return m_image ? m_image->GetHeight() : 0; }

        VkFramebuffer GetVkFramebuffer() const { return m_vkFramebuffer; }
        CommandBuffer& GetCommandBuffer() { return *m_commandBuffer; }
        Fence& GetFence() const { return *m_fence; }
        VkSemaphore GetAvailableSemaphore() const { return m_imageAvailableSemaphore; }
        VkSemaphore GetRenderFinishedSemaphore() const { return m_renderFinishedSemaphore; }
        IImage* GetImage() override { return m_image.get(); }
        uint32_t GetSwapchainIndex() const { return m_swapchainIndex; }
        VkExtent2D GetVkExtent() const;
        VkFormat GetVkFormat() const;
        Image& GetVkImage();

    protected:
        SurfaceContext& m_context;
        VkFramebuffer m_vkFramebuffer = VK_NULL_HANDLE;
        std::unique_ptr<CommandBuffer> m_commandBuffer;
        std::unique_ptr<Image> m_image;

        std::unique_ptr<Fence> m_fence;
        VkSemaphore m_imageAvailableSemaphore = VK_NULL_HANDLE;
        VkSemaphore m_renderFinishedSemaphore = VK_NULL_HANDLE;

        uint32_t m_swapchainIndex = 0;

        void CreateFramebufferResource(VkRenderPass renderPass);
    };
}

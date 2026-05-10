#pragma once

#include "moth_graphics/graphics/image.h"
#include "moth_graphics/graphics/itarget.h"
#include "vulkan_fence.h"
#include "moth_graphics/graphics/vulkan/vulkan_surface_context.h"
#include "vulkan_texture.h"

#include <vulkan/vulkan_core.h>

#include <memory>
#include <cstdint>

namespace moth_graphics::graphics::vulkan {
    class CommandBuffer;
    struct FrameSlot;

    class Framebuffer : public ITarget {
    public:
        Framebuffer(SurfaceContext& context, uint32_t width, uint32_t height, VkImage image, VkImageView view, VkFormat format, VkRenderPass renderPass, uint32_t swapchainIndex);
        Framebuffer(SurfaceContext& context, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkRenderPass renderPass);
        virtual ~Framebuffer();

        int GetWidth() const { return m_texture ? m_texture->GetWidth() : 0; }
        int GetHeight() const { return m_texture ? m_texture->GetHeight() : 0; }

        VkFramebuffer GetVkFramebuffer() const { return m_vkFramebuffer; }
        CommandBuffer& GetCommandBuffer() { return *m_commandBuffer; }
        Image GetImage() const override { return Image{ m_texture }; }
        Texture& GetVkTexture() { return *m_texture; }
        uint32_t GetSwapchainIndex() const { return m_swapchainIndex; }
        VkExtent2D GetVkExtent() const;
        VkFormat GetVkFormat() const;

        void SetFrameSlot(std::shared_ptr<FrameSlot> slot) { m_frameSlot = slot; }
        VkSemaphore GetAvailableSemaphore() const;
        VkSemaphore GetRenderFinishedSemaphore() const;
        Fence& GetFence() const { return *m_fence; }

    protected:
        SurfaceContext& m_context;
        VkFramebuffer m_vkFramebuffer = VK_NULL_HANDLE;
        std::unique_ptr<CommandBuffer> m_commandBuffer;
        std::shared_ptr<Texture> m_texture;
        std::unique_ptr<Fence> m_fence;
        std::shared_ptr<FrameSlot> m_frameSlot;

        uint32_t m_swapchainIndex = 0;

        void CreateFramebufferResource(VkRenderPass renderPass);
    };
}

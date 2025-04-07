#pragma once

#include "canyon/graphics/vulkan/vulkan_surface_context.h"
#include "canyon/graphics/vulkan/vulkan_framebuffer.h"
#include "canyon/graphics/vulkan/vulkan_renderpass.h"

#include <vulkan/vulkan_core.h>

#include <memory>
#include <vector>
#include <cstdint>

namespace canyon::graphics::vulkan {
    class Swapchain {
    public:
        Swapchain(SurfaceContext& context, RenderPass& renderPass, VkSurfaceKHR surface, VkExtent2D extent);
        ~Swapchain();

        VkExtent2D GetExtent() const { return m_extent; }

        Framebuffer* GetNextFramebuffer();

        VkSwapchainKHR GetVkSwapchain() const { return m_vkSwapchain; }

        uint32_t GetImageCount() const { return m_imageCount; }

    private:
        SurfaceContext& m_context;
        VkExtent2D m_extent;
        VkSwapchainKHR m_vkSwapchain;
        std::vector<std::unique_ptr<Framebuffer>> m_framebuffers;
        uint32_t m_currentFrame = 0;
        uint32_t m_imageCount = 0;
    };
}

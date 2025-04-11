#pragma once

#include "canyon/graphics/surface_context.h"
#include "canyon/graphics/vulkan/vulkan_context.h"
#include "canyon/graphics/itexture.h"
#include "canyon/graphics/iimage.h"
#include "canyon/graphics/ifont.h"
#include "canyon/utils/rect.h"

#include <vulkan/vulkan_core.h>
#include <vk_mem_alloc.h>

#include <memory>
#include <cstdint>
#include <filesystem>

namespace canyon::graphics::vulkan {
    class SurfaceContext : public graphics::SurfaceContext {
    public:
        SurfaceContext(Context& context);
        virtual ~SurfaceContext();

        Context& GetContext() const override { return m_context; }

        std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture) override;
        std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) override;
        std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, uint32_t size) override;
        std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) override;
        std::unique_ptr<IImage> ImageFromFile(std::filesystem::path const& path) override;

        VkPhysicalDevice const& GetVkPhysicalDevice() const { return m_vkPhysicalDevice; }
        uint32_t GetVkQueueFamily() const { return m_vkQueueFamily; }
        VkDevice const& GetVkDevice() const { return m_vkDevice; }
        VkQueue const& GetVkQueue() const { return m_vkQueue; }
        VkDescriptorPool const& GetVkDescriptorPool() const { return m_vkDescriptorPool; }
        VkCommandPool const& GetVkCommandPool() const { return m_vkCommandPool; }
        VmaAllocator const& GetVmaAllocator() const { return m_vmaAllocator; }
        VkPhysicalDeviceProperties const& GetVkPhysicalDeviceProperties() const { return m_vkDeviceProperties; }

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        static VkSurfaceFormatKHR selectSurfaceFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkFormat* request_formats, int request_formats_count, VkColorSpaceKHR request_color_space);
        static VkPresentModeKHR selectPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkPresentModeKHR* request_modes, int request_modes_count);
        static int getMinImageCountFromPresentMode(VkPresentModeKHR present_mode);

    private:
        Context& m_context;

        VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
        uint32_t m_vkQueueFamily = static_cast<uint32_t>(-1);
        VkDevice m_vkDevice = VK_NULL_HANDLE;
        VkQueue m_vkQueue = VK_NULL_HANDLE;
        VkDescriptorPool m_vkDescriptorPool = VK_NULL_HANDLE;
        VkCommandPool m_vkCommandPool = VK_NULL_HANDLE;
        VmaAllocator m_vmaAllocator = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties m_vkDeviceProperties;
    };
}

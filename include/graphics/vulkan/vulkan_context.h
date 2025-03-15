#pragma once

#include "graphics/context.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace graphics::vulkan {
    class Context : public graphics::Context {
    public:
        Context();
        virtual ~Context();

        std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) override;
        std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture) override;
        std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) override;
        std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, int size) override;

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        VkInstance m_vkInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_vkDebugMessenger = VK_NULL_HANDLE;
        VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
        uint32_t m_vkQueueFamily = static_cast<uint32_t>(-1);
        VkDevice m_vkDevice = VK_NULL_HANDLE;
        VkQueue m_vkQueue = VK_NULL_HANDLE;
        VkDescriptorPool m_vkDescriptorPool = VK_NULL_HANDLE;
        VkCommandPool m_vkCommandPool = VK_NULL_HANDLE;
        VmaAllocator m_vmaAllocator = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties m_vkDeviceProperties;

        FT_Library m_ftLibrary;

        static VkSurfaceFormatKHR selectSurfaceFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkFormat* request_formats, int request_formats_count, VkColorSpaceKHR request_color_space);
        static VkPresentModeKHR selectPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkPresentModeKHR* request_modes, int request_modes_count);
        static int getMinImageCountFromPresentMode(VkPresentModeKHR present_mode);
    };
}

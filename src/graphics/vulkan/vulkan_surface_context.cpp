#include "common.h"
#include "canyon/graphics/vulkan/vulkan_surface_context.h"
#include "canyon/graphics/vulkan/vulkan_context.h"
#include "canyon/graphics/vulkan/vulkan_font.h"
#include "canyon/graphics/vulkan/vulkan_image.h"
#include "canyon/graphics/vulkan/vulkan_texture.h"
#include "canyon/graphics/vulkan/vulkan_utils.h"

namespace {
    char const* deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
}

namespace graphics::vulkan {
    SurfaceContext::SurfaceContext(Context& context)
        : m_context(context) {

        // select device
        {
            uint32_t gpuCount;
            CHECK_VK_RESULT(vkEnumeratePhysicalDevices(m_context.GetInstance(), &gpuCount, nullptr));
            std::vector<VkPhysicalDevice> gpus(gpuCount);
            CHECK_VK_RESULT(vkEnumeratePhysicalDevices(m_context.GetInstance(), &gpuCount, gpus.data()));

            uint32_t selectedGpu = 0;
            for (uint32_t i = 0; i < gpuCount; ++i) {
                VkPhysicalDeviceFeatures features;
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(gpus[i], &properties);
                vkGetPhysicalDeviceFeatures(gpus[i], &features);
                if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && features.samplerAnisotropy == VK_TRUE) {
                    selectedGpu = i;
                    m_vkDeviceProperties = properties;
                    break;
                }
            }

            m_vkPhysicalDevice = gpus[selectedGpu];
        }

        // queue family
        {
            uint32_t queueCount;
            vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueCount, nullptr);
            std::vector<VkQueueFamilyProperties> queues(queueCount);
            vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueCount, queues.data());

            for (uint32_t i = 0; i < queueCount; ++i) {
                if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    m_vkQueueFamily = i;
                    break;
                }
            }
        }

        // create device
        {
            int deviceExtensionCount = 1;
            float const queuePriority[] = { 1.0f };
            VkDeviceQueueCreateInfo queueInfo[1] = {};
            queueInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo[0].queueFamilyIndex = m_vkQueueFamily;
            queueInfo[0].queueCount = 1;
            queueInfo[0].pQueuePriorities = queuePriority;
            VkPhysicalDeviceFeatures deviceFeatures{};
            deviceFeatures.samplerAnisotropy = VK_TRUE;
            VkDeviceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.queueCreateInfoCount = IM_ARRAYSIZE(queueInfo);
            createInfo.pQueueCreateInfos = queueInfo;
            createInfo.enabledExtensionCount = deviceExtensionCount;
            createInfo.ppEnabledExtensionNames = deviceExtensions;
            createInfo.pEnabledFeatures = &deviceFeatures;
            CHECK_VK_RESULT(vkCreateDevice(m_vkPhysicalDevice, &createInfo, nullptr, &m_vkDevice));
            vkGetDeviceQueue(m_vkDevice, m_vkQueueFamily, 0, &m_vkQueue);
        }

        // descriptor pool
        {
            VkDescriptorPoolSize poolSizes[] = {
                { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 },
            };
            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
            poolInfo.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(poolSizes));
            poolInfo.pPoolSizes = poolSizes;
            CHECK_VK_RESULT(vkCreateDescriptorPool(m_vkDevice, &poolInfo, nullptr, &m_vkDescriptorPool));
        }

        // command pool
        {
            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = m_vkQueueFamily;

            CHECK_VK_RESULT(vkCreateCommandPool(m_vkDevice, &poolInfo, nullptr, &m_vkCommandPool));
        }

        // allocator
        {
            VmaAllocatorCreateInfo allocatorCreateInfo{};
            allocatorCreateInfo.instance = m_context.GetInstance();
            allocatorCreateInfo.physicalDevice = m_vkPhysicalDevice;
            allocatorCreateInfo.device = m_vkDevice;
            vmaCreateAllocator(&allocatorCreateInfo, &m_vmaAllocator);
        }
    }

    std::unique_ptr<IImage> SurfaceContext::NewImage(std::shared_ptr<ITexture> texture) {
        auto vulkanTexture = std::dynamic_pointer_cast<Texture>(texture);
        return std::make_unique<Image>(vulkanTexture);
    }

    std::unique_ptr<IImage> SurfaceContext::NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) {
        auto vulkanTexture = std::dynamic_pointer_cast<Texture>(texture);
        return std::make_unique<Image>(vulkanTexture, sourceRect);
    }

    std::unique_ptr<IFont> SurfaceContext::FontFromFile(std::filesystem::path const& path, uint32_t size) {
        return Font::Load(path, size, *this);
    }

    std::unique_ptr<ITexture> SurfaceContext::TextureFromFile(std::filesystem::path const& path) {
        return Texture::FromFile(*this, path);
    }

    std::unique_ptr<IImage> SurfaceContext::ImageFromFile(std::filesystem::path const& path) {
        std::shared_ptr<Texture> texture(static_cast<Texture*>(TextureFromFile(path).release()));
        return std::make_unique<Image>(texture);
    }

    SurfaceContext::~SurfaceContext() {
        vkDeviceWaitIdle(m_vkDevice);
        vkDestroyDescriptorPool(m_vkDevice, m_vkDescriptorPool, nullptr);
        vkDestroyCommandPool(m_vkDevice, m_vkCommandPool, nullptr);
        vmaDestroyAllocator(m_vmaAllocator);
        vkDestroyDevice(m_vkDevice, nullptr);
    }

    VkCommandBuffer SurfaceContext::beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_vkCommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(m_vkDevice, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    void SurfaceContext::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(m_vkQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_vkQueue);

        vkFreeCommandBuffers(m_vkDevice, m_vkCommandPool, 1, &commandBuffer);
    }

    VkSurfaceFormatKHR SurfaceContext::selectSurfaceFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkFormat* request_formats, int request_formats_count, VkColorSpaceKHR request_color_space) {
        uint32_t avail_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &avail_count, NULL);
        ImVector<VkSurfaceFormatKHR> avail_format;
        avail_format.resize((int)avail_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &avail_count, avail_format.Data);

        // First check if only one format, VK_FORMAT_UNDEFINED, is available, which would imply that any format is available
        if (avail_count == 1) {
            if (avail_format[0].format == VK_FORMAT_UNDEFINED) {
                VkSurfaceFormatKHR ret;
                ret.format = request_formats[0];
                ret.colorSpace = request_color_space;
                return ret;
            } else {
                // No point in searching another format
                return avail_format[0];
            }
        } else {
            // Request several formats, the first found will be used
            for (int request_i = 0; request_i < request_formats_count; request_i++)
                for (uint32_t avail_i = 0; avail_i < avail_count; avail_i++)
                    if (avail_format[avail_i].format == request_formats[request_i] && avail_format[avail_i].colorSpace == request_color_space)
                        return avail_format[avail_i];

            // If none of the requested image formats could be found, use the first available
            return avail_format[0];
        }
    }

    VkPresentModeKHR SurfaceContext::selectPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkPresentModeKHR* request_modes, int request_modes_count) {
        // Request a certain mode and confirm that it is available. If not use VK_PRESENT_MODE_FIFO_KHR which is mandatory
        uint32_t avail_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &avail_count, NULL);
        ImVector<VkPresentModeKHR> avail_modes;
        avail_modes.resize((int)avail_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &avail_count, avail_modes.Data);

        for (int request_i = 0; request_i < request_modes_count; request_i++)
            for (uint32_t avail_i = 0; avail_i < avail_count; avail_i++)
                if (request_modes[request_i] == avail_modes[avail_i])
                    return request_modes[request_i];

        return VK_PRESENT_MODE_FIFO_KHR; // Always available
    }

    int SurfaceContext::getMinImageCountFromPresentMode(VkPresentModeKHR present_mode) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return 3;
        if (present_mode == VK_PRESENT_MODE_FIFO_KHR || present_mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
            return 2;
        if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            return 1;
        IM_ASSERT(0);
        return 1;
    }
}

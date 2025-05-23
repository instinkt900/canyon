#pragma once

#include "canyon/graphics/vulkan/vulkan_surface_context.h"

#include <vulkan/vulkan_core.h>

#include <stddef.h>

namespace canyon::graphics::vulkan {
    class Buffer {
    public:
        Buffer(SurfaceContext& context, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
        ~Buffer();

        size_t GetSize() const { return m_size; }

        void* Map();
        void Unmap();

        void Copy(Buffer& sourceBuffer);

        VkBuffer GetVKBuffer() const { return m_vkBuffer; }

    private:
        SurfaceContext& m_context;
        VkBuffer m_vkBuffer;
        VmaAllocation m_vmaAllocation;
        size_t m_size;
    };
}

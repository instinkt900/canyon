#pragma once

#include "moth_graphics/graphics/vulkan/vulkan_surface_context.h"
#include "vulkan_unique.h"

#include <vulkan/vulkan_core.h>

namespace moth_graphics::graphics::vulkan {
    class Fence {
    public:
        Fence(SurfaceContext& context);
        ~Fence() = default;

        VkFence GetVkFence() const { return m_vkFence; }

    private:
        SurfaceContext& m_context;
        UniqueHandle<VkFence> m_vkFence;
    };
}

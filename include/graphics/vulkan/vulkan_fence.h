#pragma once

#include "vulkan_surface_context.h"

#include <vulkan/vulkan.hpp>

namespace graphics::vulkan {
    class Fence {
    public:
        Fence(SurfaceContext& context);
        ~Fence();

        VkFence GetVkFence() const { return m_vkFence; }

    private:
        SurfaceContext& m_context;
        VkFence m_vkFence;
    };
}

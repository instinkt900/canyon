#include "common.h"
#include "canyon/graphics/vulkan/vulkan_fence.h"

namespace canyon::graphics::vulkan {
    Fence::Fence(SurfaceContext& context)
        : m_context(context) {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        vkCreateFence(m_context.GetVkDevice(), &fenceInfo, nullptr, &m_vkFence);
    }

    Fence::~Fence() {
        vkDestroyFence(m_context.GetVkDevice(), m_vkFence, nullptr);
    }
}

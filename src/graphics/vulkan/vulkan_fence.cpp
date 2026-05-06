#include "vulkan_utils.h"
#include "common.h"
#include "vulkan_fence.h"

namespace moth_graphics::graphics::vulkan {
    Fence::Fence(SurfaceContext& context)
        : m_context(context)
        , m_vkFence(VK_NULL_HANDLE) {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        CHECK_VK_RESULT(vkCreateFence(m_context.GetVkDevice(), &fenceInfo, nullptr, &m_vkFence));
    }

    Fence::~Fence() {
        vkDestroyFence(m_context.GetVkDevice(), m_vkFence, nullptr);
    }
}

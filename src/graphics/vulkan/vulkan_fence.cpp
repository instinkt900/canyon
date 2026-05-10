#include "vulkan_utils.h"
#include "common.h"
#include "vulkan_fence.h"

namespace moth_graphics::graphics::vulkan {
    Fence::Fence(SurfaceContext& context)
        : m_context(context) {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkFence fence = VK_NULL_HANDLE;
        CHECK_VK_RESULT(vkCreateFence(m_context.GetVkDevice(), &fenceInfo, nullptr, &fence));
        VkDevice const device = m_context.GetVkDevice();
        m_vkFence = UniqueHandle<VkFence>(fence, [device](VkFence h) { vkDestroyFence(device, h, nullptr); });
    }
}

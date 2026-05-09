#pragma once

#include "moth_graphics/graphics/context.h"
#include "moth_graphics/graphics/vulkan/vulkan_context.h"

namespace moth_graphics::graphics::vulkan {
    /// @brief Moth-managed Vulkan context.
    ///
    /// Creates and owns a VkInstance and FT_Library via Startup(), tears them
    /// down via Shutdown().  The result can be used as a plain Context&.
    class ManagedContext : public graphics::Context {
    public:
        ManagedContext() = default;
        ~ManagedContext() override = default;

        bool Startup() override;
        void Shutdown() override;

        moth_graphics::graphics::vulkan::Context& GetContext() { return m_context; }
        moth_graphics::graphics::vulkan::Context const& GetContext() const { return m_context; }

    private:
        moth_graphics::graphics::vulkan::Context m_context{VK_NULL_HANDLE, nullptr};
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    };
}

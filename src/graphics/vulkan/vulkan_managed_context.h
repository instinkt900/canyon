#pragma once

#include "moth_graphics/graphics/vulkan/vulkan_context.h"

namespace moth_graphics::graphics::vulkan {
    /// @brief Moth-managed Vulkan context.
    ///
    /// Creates and owns a VkInstance and FT_Library via Startup(), tears them
    /// down via Shutdown().  The result can be used as a plain Context&.
    class ManagedContext {
    public:
        bool Startup();
        void Shutdown();

        Context& GetContext() { return m_context; }
        Context const& GetContext() const { return m_context; }

    private:
        Context m_context;
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    };
}

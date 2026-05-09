#pragma once

#include "moth_graphics/graphics/igraphics.h"

#include <vulkan/vulkan_core.h>

#include <memory>
#include <cstdint>

namespace moth_graphics::graphics::vulkan {
    class SurfaceContext;

    /// @brief Create a Vulkan graphics instance for standalone use.
    ///
    /// Constructs the Vulkan backend renderer without requiring Window or
    /// Application. The caller owns the surface and is responsible for
    /// presenting the swapchain images (via IGraphics::End) and recreating
    /// the swapchain on resize (via vulkan::Graphics::OnResize).
    ///
    /// @param surfaceContext  Initialized Vulkan surface context.
    /// @param surface         Vulkan surface to render into.
    /// @param surfaceWidth    Initial surface width in pixels.
    /// @param surfaceHeight   Initial surface height in pixels.
    /// @returns A fully initialized graphics instance.
    std::unique_ptr<IGraphics> CreateGraphics(
        SurfaceContext& surfaceContext,
        VkSurfaceKHR surface,
        uint32_t surfaceWidth,
        uint32_t surfaceHeight);
}

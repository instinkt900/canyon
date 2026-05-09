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
    /// Application. Begin() self-heals when the swapchain is unavailable
    /// (e.g. window minimised) — callers do not need to handle resize.
    ///
    /// Power users can interleave their own Vulkan commands via
    /// vulkan::Graphics::Flush() and GetCurrentCommandBuffer() (accessible
    /// by downcasting the returned IGraphics).
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

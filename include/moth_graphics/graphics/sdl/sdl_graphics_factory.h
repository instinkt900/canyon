#pragma once

#include "moth_graphics/graphics/igraphics.h"

#include <memory>

namespace moth_graphics::graphics::sdl {
    class SurfaceContext;

    /// @brief Create an SDL graphics instance for standalone use.
    ///
    /// Constructs the SDL backend renderer without requiring Window or
    /// Application. The caller owns the SDL renderer and window.
    ///
    /// @param surfaceContext  Initialized SDL surface context.
    /// @returns A fully initialized graphics instance.
    std::unique_ptr<IGraphics> CreateGraphics(SurfaceContext& surfaceContext);
}

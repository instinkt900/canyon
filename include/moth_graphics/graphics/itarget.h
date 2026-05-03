#pragma once

#include "moth_graphics/graphics/iimage.h"

namespace moth_graphics::graphics {
    /// @brief Abstract off-screen render target.
    ///
    /// Created via @c IGraphics::CreateTarget(). Activate it with
    /// @c IGraphics::SetTarget() to redirect draw calls, then read back
    /// the result through @c GetImage() to use it in subsequent draws.
    class ITarget {
    public:
        virtual ~ITarget() = default;

        /// @brief Returns an image view of the target's rendered content.
        ///        The returned image shares the target's underlying texture.
        virtual Image GetImage() const = 0;
    };
}

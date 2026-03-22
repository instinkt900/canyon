#pragma once

namespace moth_graphics::graphics {
    /// @brief Abstract handle to a loaded font.
    ///
    /// Holds a reference to a backend-specific font resource. Obtain instances
    /// via @c FontFactory::GetFont() and pass them to @c IGraphics::DrawText().
    class IFont {
    public:
        virtual ~IFont() = default;
    };
}

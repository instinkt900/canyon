#pragma once

#include "moth_graphics/utils/vector.h"

#include <string_view>

namespace moth_graphics::graphics {
    /// @brief Abstract handle to a loaded font.
    ///
    /// Holds a reference to a backend-specific font resource. Obtain instances
    /// via @c FontFactory::GetFont() and pass them to @c IGraphics::DrawText().
    class IFont {
    public:
        virtual ~IFont() = default;

        /// @brief Measure the pixel dimensions of a string rendered with this font.
        /// @param text UTF-8 text to measure.
        /// @return Width and height in pixels.
        virtual IntVec2 Measure(std::string_view text) const = 0;

        /// @brief Returns the distance between consecutive baselines in pixels.
        virtual int GetLineHeight() const = 0;

        /// @brief Returns the distance from the baseline to the top of the tallest glyph in pixels.
        virtual int GetAscent() const = 0;

        /// @brief Returns the distance from the baseline to the bottom of the lowest descender in pixels.
        virtual int GetDescent() const = 0;
    };
}

#pragma once

#include "graphics/color.h"
#include "moth_ui/utils/color.h"
#include "utils/rect.h"

// returns a rect with the coordinates of b within a
inline IntRect MergeRects(IntRect const& a, IntRect const& b) {
    IntRect c;
    c.topLeft = a.topLeft + b.topLeft;
    c.bottomRight = b.bottomRight + a.topLeft;
    // contain c within a
    c.topLeft.x = std::max(c.topLeft.x, a.topLeft.x);
    c.topLeft.y = std::max(c.topLeft.y, a.topLeft.y);
    c.bottomRight.x = std::min(c.bottomRight.x, a.bottomRight.x);
    c.bottomRight.y = std::min(c.bottomRight.y, a.bottomRight.y);
    return c;
}

struct ColorComponents {
    explicit ColorComponents(graphics::Color const& color)
        : r(static_cast<uint8_t>(255 * std::clamp(color.r, 0.0f, 1.0f)))
        , g(static_cast<uint8_t>(255 * std::clamp(color.g, 0.0f, 1.0f)))
        , b(static_cast<uint8_t>(255 * std::clamp(color.b, 0.0f, 1.0f)))
        , a(static_cast<uint8_t>(255 * std::clamp(color.a, 0.0f, 1.0f))) {
    }

    explicit ColorComponents(moth_ui::Color const& color)
        : r(static_cast<uint8_t>(255 * std::clamp(color.r, 0.0f, 1.0f)))
        , g(static_cast<uint8_t>(255 * std::clamp(color.g, 0.0f, 1.0f)))
        , b(static_cast<uint8_t>(255 * std::clamp(color.b, 0.0f, 1.0f)))
        , a(static_cast<uint8_t>(255 * std::clamp(color.a, 0.0f, 1.0f))) {
    }

    uint8_t r, g, b, a;
};

#pragma once

#include "canyon/utils/vector.h"
#include "canyon/graphics/blend_mode.h"

#include <algorithm>
#include <cstdint>
#include <cmath>

namespace graphics {
    using Color = Vector<float, 4>;

    namespace BasicColors {
        static Color const Red{ 1.0f, 0.0f, 0.0f, 1.0f };
        static Color const Green{ 0.0f, 1.0f, 0.0f, 1.0f };
        static Color const Blue{ 0.0f, 0.0f, 1.0f, 1.0f };
        static Color const White{ 1.0f, 1.0f, 1.0f, 1.0f };
        static Color const Black{ 0.0f, 0.0f, 0.0f, 1.0f };
    }

    // scales all components equally so the largest is 1
    inline Color Normalize(Color const& color) {
        auto const maxComponent = std::max({ color.r, color.g, color.b, color.a });
        auto const factor = 1.0f / maxComponent;
        return color * factor;
    }

    // like Normalize but only scales down if the largest component is > 1
    inline Color Limit(Color const& color) {
        auto const maxComponent = std::max({ color.r, color.g, color.b, color.a });
        if (maxComponent > 1.0f) {
            return Normalize(color);
        }
        return color;
    }

    // clamps all components to 0-1
    inline Color Clamp(Color const& color) {
        return Color{ std::clamp(color.r, 0.0f, 1.0f), std::clamp(color.g, 0.0f, 1.0f), std::clamp(color.b, 0.0f, 1.0f), std::clamp(color.a, 0.0f, 1.0f) };
    }

    inline Color FromARGB(uint32_t argb) {
        uint8_t const a = (argb & 0xFF000000) >> 24;
        uint8_t const r = (argb & 0x00FF0000) >> 16;
        uint8_t const g = (argb & 0x0000FF00) >> 8;
        uint8_t const b = (argb & 0x000000FF);
        return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }

    inline Color FromRGBA(uint32_t rgba) {
        uint8_t const r = (rgba & 0xFF000000) >> 24;
        uint8_t const g = (rgba & 0x00FF0000) >> 16;
        uint8_t const b = (rgba & 0x0000FF00) >> 8;
        uint8_t const a = (rgba & 0x000000FF);
        return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }

    inline uint32_t ToRGBA(Color const& color) {
        auto const limitedColor = Clamp(color);
        uint32_t const r = static_cast<uint32_t>(std::round(limitedColor.r * 255));
        uint32_t const g = static_cast<uint32_t>(std::round(limitedColor.g * 255));
        uint32_t const b = static_cast<uint32_t>(std::round(limitedColor.b * 255));
        uint32_t const a = static_cast<uint32_t>(std::round(limitedColor.a * 255));
        return (r << 24) | (g << 16) | (b << 8) | a;
    }

    inline uint32_t ToARGB(Color const& color) {
        auto const limitedColor = Clamp(color);
        uint32_t const r = static_cast<uint32_t>(std::round(limitedColor.r * 255));
        uint32_t const g = static_cast<uint32_t>(std::round(limitedColor.g * 255));
        uint32_t const b = static_cast<uint32_t>(std::round(limitedColor.b * 255));
        uint32_t const a = static_cast<uint32_t>(std::round(limitedColor.a * 255));
        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    inline uint32_t ToABGR(Color const& color) {
        auto const limitedColor = Clamp(color);
        uint32_t const r = static_cast<uint32_t>(std::round(limitedColor.r * 255));
        uint32_t const g = static_cast<uint32_t>(std::round(limitedColor.g * 255));
        uint32_t const b = static_cast<uint32_t>(std::round(limitedColor.b * 255));
        uint32_t const a = static_cast<uint32_t>(std::round(limitedColor.a * 255));
        return (a << 24) | (b << 16) | (g << 8) | r;
    }

    inline Color Blend(Color const& srcColor, Color const& dstColor, BlendMode blend) {
        Color srcColorFactor, dstColorFactor;
        float srcAlphaFactor, dstAlphaFactor;
        switch (blend) {
        default:
        case BlendMode::Replace:
            srcColorFactor = { 1.0f, 1.0f, 1.0f, 0.0f };
            dstColorFactor = { 0.0f, 0.0f, 0.0f, 0.0f };
            srcAlphaFactor = 1.0f;
            dstAlphaFactor = 0.0f;
            break;
        case BlendMode::Add:
            srcColorFactor = { srcColor.a, srcColor.a, srcColor.a, 0.0f };
            dstColorFactor = { 1.0f, 1.0f, 1.0f, 1.0f };
            srcAlphaFactor = 0.0f;
            dstAlphaFactor = 1.0f;
            break;
        case BlendMode::Alpha:
            srcColorFactor = { srcColor.a, srcColor.a, srcColor.a, 0 };
            dstColorFactor = { 1.0f - srcColor.a, 1.0f - srcColor.a, 1.0f - srcColor.a, 0.0f };
            srcAlphaFactor = 1.0f;
            dstAlphaFactor = 1.0f - srcColor.a;
            break;
        case BlendMode::Modulate:
            srcColorFactor = { 0.0f, 0.0f, 0.0f, 0.0f };
            dstColorFactor = srcColor;
            srcAlphaFactor = 0.0f;
            dstAlphaFactor = 1.0f;
            break;
        case BlendMode::Multiply:
            srcColorFactor = dstColor;
            dstColorFactor = { 1.0f - srcColor.a, 1.0f - srcColor.a, 1.0f - srcColor.a, 0.0f };
            srcAlphaFactor = dstColor.a;
            dstAlphaFactor = 1.0f - srcColor.a;
            break;
        }
        float const finalAlpha = (srcColor.a * srcAlphaFactor) + (dstColor.a * dstAlphaFactor);
        Color finalColor = (srcColor * srcColorFactor) + (dstColor * dstColorFactor);
        finalColor.a = finalAlpha;
        return finalColor;
    }
}

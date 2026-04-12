#pragma once

// fmt/spdlog formatters for moth_ui types.
// Placed in canyon to keep moth_ui free of formatting dependencies.
//
// Include this header wherever you want to use moth_ui types in fmt/spdlog calls.
// It supersedes rect_format.h — including both is safe (rect_format.h includes
// this header), but including just this one is sufficient.

#include <spdlog/fmt/fmt.h>
#include <magic_enum.hpp>

#include <moth_ui/utils/vector.h>
#include <moth_ui/utils/rect.h>
#include <moth_ui/utils/color.h>
#include <moth_ui/layout/layout_rect.h>
#include <moth_ui/graphics/blend_mode.h>
#include <moth_ui/graphics/image_scale_type.h>
#include <moth_ui/graphics/text_alignment.h>
#include <moth_ui/utils/interp.h>
#include <moth_ui/animation/animation_track.h>
#include <moth_ui/animation/animation_clip.h>

// ---------------------------------------------------------------------------
// Vector<T, Dim>
// Formats as (x, y), (x, y, z), or (x, y, z, w) for the common dimensions.
// ---------------------------------------------------------------------------

template <typename T, int Dim>
struct fmt::formatter<moth_ui::Vector<T, Dim>> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    auto format(moth_ui::Vector<T, Dim> const& v, fmt::format_context& ctx) const {
        if constexpr (Dim == 2) {
            return fmt::format_to(ctx.out(), "({}, {})", v.x, v.y);
        } else if constexpr (Dim == 3) {
            return fmt::format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
        } else if constexpr (Dim == 4) {
            return fmt::format_to(ctx.out(), "({}, {}, {}, {})", v.x, v.y, v.z, v.w);
        } else {
            auto out = ctx.out();
            *out++ = '(';
            for (int i = 0; i < Dim; ++i) {
                if (i > 0) { *out++ = ','; *out++ = ' '; }
                out = fmt::format_to(out, "{}", v.data[i]);
            }
            *out++ = ')';
            return out;
        }
    }
};

// ---------------------------------------------------------------------------
// Rect<T>
// Formats as [topLeft → bottomRight], e.g. [(0, 0) → (100, 200)].
// ---------------------------------------------------------------------------

template <typename T>
struct fmt::formatter<moth_ui::Rect<T>> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    auto format(moth_ui::Rect<T> const& r, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "[{} -> {}]", r.topLeft, r.bottomRight);
    }
};

// ---------------------------------------------------------------------------
// LayoutRect
// Formats anchor and offset as separate rects, e.g.:
//   {anchor: [(0, 0) → (1, 1)], offset: [(0, 0) → (0, 0)]}
// ---------------------------------------------------------------------------

template <>
struct fmt::formatter<moth_ui::LayoutRect> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    auto format(moth_ui::LayoutRect const& lr, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(),
            "{{anchor: {}, offset: {}}}",
            lr.anchor, lr.offset);
    }
};

// ---------------------------------------------------------------------------
// Enums — all moth_ui enums via magic_enum.
// Covers: BlendMode, ImageScaleType, TextHorizAlignment, TextVertAlignment,
//         AnimationTrack::Target, InterpType, AnimationClip::LoopType.
//
// Uses the SFINAE second-parameter pattern supported by fmtlib. If a more
// specific formatter is ever defined for a particular enum it will take
// precedence over this general one.
// ---------------------------------------------------------------------------

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_enum_v<T>, char>> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    auto format(T value, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}::{}", magic_enum::enum_type_name<T>(), magic_enum::enum_name(value));
    }
};

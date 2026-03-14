#pragma once

#include <canyon/utils/rect.h>
#include <spdlog/fmt/fmt.h>

template <typename T>
struct fmt::formatter<moth_ui::Rect<T>> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(moth_ui::Rect<T> const& r, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "(x={}, y={}, w={}, h={})", r.x(), r.y(), r.w(), r.h());
    }
};

#pragma once

#include "canyon/graphics/blend_mode.h"
#include "canyon/graphics/text_alignment.h"
#include "canyon/utils/rect.h"
#include "canyon/utils/vector.h"

#include <moth_ui/graphics/blend_mode.h>
#include <moth_ui/graphics/text_alignment.h>
#include <moth_ui/utils/rect.h>
#include <moth_ui/utils/vector.h>

namespace canyon {
    // inline graphics::BlendMode FromMothUI(moth_ui::BlendMode blend) {
    //     switch (blend) {
    //     case moth_ui::BlendMode::Modulate: {
    //         return graphics::BlendMode::Modulate;
    //     }
    //     case moth_ui::BlendMode::Multiply: {
    //         return graphics::BlendMode::Multiply;
    //     }
    //     case moth_ui::BlendMode::Add: {
    //         return graphics::BlendMode::Add;
    //     }
    //     case moth_ui::BlendMode::Alpha: {
    //         return graphics::BlendMode::Alpha;
    //     }
    //     case moth_ui::BlendMode::Replace: {
    //         return graphics::BlendMode::Replace;
    //     }
    //     default: {
    //         return graphics::BlendMode::Invalid;
    //     }
    //     }
    // }
    //
    // template <typename T, int Dim>
    // inline moth_ui::Vector<T, Dim> ToMothUI(Vector<T, Dim> const& a) {
    //     moth_ui::Vector<T, Dim> b{};
    //     for (int i = 0; i < Dim; ++i) {
    //         b.data[i] = a.data[i];
    //     }
    //     return b;
    // }
    //
    // template <typename T, int Dim>
    // inline Vector<T, Dim> FromMothUI(moth_ui::Vector<T, Dim> const& a) {
    //     Vector<T, Dim> b{};
    //     for (int i = 0; i < Dim; ++i) {
    //         b.data[i] = a.data[i];
    //     }
    //     return b;
    // }
    //
    // template <typename T>
    // inline moth_ui::Rect<T> ToMothUI(Rect<T> const& r) {
    //     return { ToMothUI(r.topLeft), ToMothUI(r.bottomRight) };
    // }
    //
    // template <typename T>
    // inline Rect<T> FromMothUI(moth_ui::Rect<T> const& r) {
    //     return { FromMothUI(r.topLeft), FromMothUI(r.bottomRight) };
    // }
    //
    // inline graphics::TextHorizAlignment FromMothUI(moth_ui::TextHorizAlignment a) {
    //     if (a == moth_ui::TextHorizAlignment::Right)
    //         return graphics::TextHorizAlignment::Right;
    //     if (a == moth_ui::TextHorizAlignment::Center)
    //         return graphics::TextHorizAlignment::Center;
    //     return graphics::TextHorizAlignment::Left;
    // }
    //
    // inline graphics::TextVertAlignment FromMothUI(moth_ui::TextVertAlignment a) {
    //     if (a == moth_ui::TextVertAlignment::Middle)
    //         return graphics::TextVertAlignment::Middle;
    //     if (a == moth_ui::TextVertAlignment::Bottom)
    //         return graphics::TextVertAlignment::Bottom;
    //     return graphics::TextVertAlignment::Top;
    // }
}

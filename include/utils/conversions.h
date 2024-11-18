#pragma once

#include "utils/rect.h"
#include "utils/vector.h"
#include "graphics/text_alignment.h"
#include <moth_ui/utils/rect.h>
#include <moth_ui/utils/vector.h>
#include <moth_ui/text_alignment.h>

template <typename T, int Dim>
inline moth_ui::Vector<T, Dim> ToMothUI(Vector<T, Dim> const& a) {
    moth_ui::Vector<T, Dim> b{};
    for (int i = 0; i < Dim; ++i) {
        b.data[i] = a.data[i];
    }
    return b;
}

template <typename T, int Dim>
inline Vector<T, Dim> FromMothUI(moth_ui::Vector<T, Dim> const& a) {
    Vector<T, Dim> b{};
    for (int i = 0; i < Dim; ++i) {
        b.data[i] = a.data[i];
    }
    return b;
}

template <typename T>
inline moth_ui::Rect<T> ToMothUI(Rect<T> const& r) {
    return { ToMothUI(r.topLeft), ToMothUI(r.bottomRight) };
}

template <typename T>
inline Rect<T> FromMothUI(moth_ui::Rect<T> const& r) {
    return { FromMothUI(r.topLeft), FromMothUI(r.bottomRight) };
}

namespace graphics {
    inline TextHorizAlignment FromMothUI(moth_ui::TextHorizAlignment a) {
        if (a == moth_ui::TextHorizAlignment::Right) return TextHorizAlignment::Right;
        if (a == moth_ui::TextHorizAlignment::Center) return TextHorizAlignment::Center;
        return TextHorizAlignment::Left;
    }

    inline TextVertAlignment FromMothUI(moth_ui::TextVertAlignment a) {
        if (a == moth_ui::TextVertAlignment::Middle) return TextVertAlignment::Middle;
        if (a == moth_ui::TextVertAlignment::Bottom) return TextVertAlignment::Bottom;
        return TextVertAlignment::Top;
    }
}


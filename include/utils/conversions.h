#pragma once

#include "utils/rect.h"
#include "utils/vector.h"
#include <moth_ui/utils/rect.h>
#include <moth_ui/utils/vector.h>

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


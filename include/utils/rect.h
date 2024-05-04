#pragma once

#include <moth_ui/utils/rect.h>
#include "vector.h"

template <typename T>
struct Rect {
    Vector<T, 2> topLeft;
    Vector<T, 2> bottomRight;

    T x() const { return topLeft.x; }
    T y() const { return topLeft.y; }
    T w() const { return bottomRight.x - topLeft.x; }
    T h() const { return bottomRight.y - topLeft.y; }
    Vector<T, 2> dim() const { return bottomRight - topLeft; }

    // cast operator
    template <typename U>
    explicit operator Rect<U>() const {
        Rect<U> result;
        result.topLeft = static_cast<Vector<U, 2>>(topLeft);
        result.bottomRight = static_cast<Vector<U, 2>>(bottomRight);
        return result;
    }
};

template <typename T>
Rect<T> MakeRect(T x, T y, T w, T h) {
    return { { x, y }, { x + w, y + h } };
}

using IntRect = Rect<int>;
using FloatRect = Rect<float>;

template <typename T>
inline bool operator==(Rect<T> const& a, Rect<T> const& b) {
    return (a.topLeft == b.topLeft) && (a.bottomRight == b.bottomRight);
}

template <typename T>
inline bool operator!=(Rect<T> const& a, Rect<T> const& b) {
    return !(a == b);
}

template <typename T>
inline Rect<T>& operator+=(Rect<T>& a, Rect<T> const& b) {
    a.topLeft += b.topLeft;
    a.bottomRight += b.bottomRight;
    return a;
}

template <typename T>
inline Rect<T>& operator-=(Rect<T>& a, Rect<T> const& b) {
    a.topLeft -= b.topLeft;
    a.bottomRight -= b.bottomRight;
    return a;
}

template <typename T>
inline Rect<T>& operator+=(Rect<T>& a, Vector<T, 2> const& b) {
    a.topLeft += b;
    a.bottomRight += b;
    return a;
}

template <typename T>
inline Rect<T>& operator-=(Rect<T>& a, Vector<T, 2> const& b) {
    a.topLeft -= b;
    a.bottomRight -= b;
    return a;
}

template <typename T>
inline Rect<T>& operator*=(Rect<T>& a, T b) {
    a.topLeft *= b;
    a.bottomRight *= b;
    return a;
}

template <typename T>
inline Rect<T>& operator/=(Rect<T>& a, T b) {
    a.topLeft /= b;
    a.bottomRight /= b;
    return a;
}

template <typename T>
inline Rect<T>& operator*=(Rect<T>& a, Vector<T, 2> const& b) {
    a.topLeft *= b;
    a.bottomRight *= b;
    return a;
}

template <typename T>
inline Rect<T>& operator/=(Rect<T>& a, Vector<T, 2> const& b) {
    a.topLeft /= b;
    a.bottomRight /= b;
    return a;
}

template <typename T>
inline Rect<T> operator+(Rect<T> const& rect, T other) {
    auto ret = rect;
    ret += other;
    return ret;
}

template <typename T>
inline Rect<T> operator-(Rect<T> const& rect, T other) {
    auto ret = rect;
    ret -= other;
    return ret;
}

template <typename T>
inline Rect<T> operator*(Rect<T> const& rect, T other) {
    auto ret = rect;
    ret *= other;
    return ret;
}

template <typename T>
inline Rect<T> operator/(Rect<T> const& rect, T other) {
    auto ret = rect;
    ret /= other;
    return ret;
}

template <typename T>
inline Rect<T> operator+(Rect<T> const& rect, Vector<T, 2> const& other) {
    auto ret = rect;
    ret += other;
    return ret;
}

template <typename T>
inline Rect<T> operator-(Rect<T> const& rect, Vector<T, 2> const& other) {
    auto ret = rect;
    ret -= other;
    return ret;
}

template <typename T>
inline Rect<T> operator*(Rect<T> const& rect, Vector<T, 2> const& other) {
    auto ret = rect;
    ret *= other;
    return ret;
}

template <typename T>
inline Rect<T> operator/(Rect<T> const& rect, Vector<T, 2> const& other) {
    auto ret = rect;
    ret /= other;
    return ret;
}

template <typename T>
inline bool IsZero(Rect<T> const& rect) {
    return rect.topLeft.x == 0 && rect.topLeft.y == 0 && rect.bottomRight.x == 0 && rect.bottomRight.y == 0;
}

template <typename T, typename U>
inline bool IsInRect(Vector<T, 2> const& point, Rect<U> const& rect) {
    if (point.x > rect.bottomRight.x || point.x < rect.topLeft.x ||
        point.y > rect.bottomRight.y || point.y < rect.topLeft.y) {
        return false;
    }
    return true;
}

template <typename T>
inline bool Intersects(Rect<T> const& rect1, Rect<T> const& rect2) {
    return !(rect2.topLeft.x > rect1.bottomRight.x || rect2.bottomRight.x < rect1.topLeft.x || rect2.topLeft.y > rect1.bottomRight.y || rect2.bottomRight.y < rect1.topLeft.y);
}

template <typename T>
inline moth_ui::Rect<T> ToMothUI(Rect<T> const& r) {
    return { ToMothUI(r.topLeft), ToMothUI(r.bottomRight) };
}


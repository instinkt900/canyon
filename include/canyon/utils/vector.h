#pragma once

#include <array>

#pragma pack(push, 1)

template <class T, int Dim>
class VectorData {
public:
    T data[Dim] = { 0 };
};

template <class T>
class VectorData<T, 2> {
public:
    union {
        struct {
            T x, y;
        };
        T data[2] = { 0 };
    };
};

template <class T>
class VectorData<T, 3> {
public:
    union {
        struct {
            T x, y, z;
        };
        struct {
            T r, g, b;
        };
        T data[3] = { 0 };
    };
};

template <class T>
class VectorData<T, 4> {
public:
    union {
        struct {
            T x, y, z, w;
        };
        struct {
            T r, g, b, a;
        };
        T data[4] = { 0 };
    };
};

template <class T, int Dim>
class Vector : public VectorData<T, Dim> {
public:
    using VectorData<T, Dim>::data;

    Vector() = default;

    template <class... Scalars, std::enable_if_t<Dim >= 2 && std::conjunction_v<std::is_convertible<Scalars, T>...> && sizeof...(Scalars) == Dim, bool> = true>
    Vector(Scalars... scalars) {
        *reinterpret_cast<std::array<T, Dim>*>(data) = { T(scalars)... };
    }

    // cast operator
    template <typename U>
    explicit operator Vector<U, Dim>() const {
        Vector<U, Dim> result;
        for (int i = 0; i < Dim; ++i) {
            result.data[i] = static_cast<U>(data[i]);
        }
        return result;
    }

    Vector<T, Dim>& operator+=(Vector<T, Dim> const& other) {
        for (int i = 0; i < Dim; ++i) {
            data[i] += other.data[i];
        }
        return *this;
    }

    Vector<T, Dim>& operator-=(Vector<T, Dim> const& other) {
        for (int i = 0; i < Dim; ++i) {
            data[i] -= other.data[i];
        }
        return *this;
    }

    Vector<T, Dim>& operator*=(Vector<T, Dim> const& other) {
        for (int i = 0; i < Dim; ++i) {
            data[i] *= other.data[i];
        }
        return *this;
    }

    Vector<T, Dim>& operator/=(Vector<T, Dim> const& other) {
        for (int i = 0; i < Dim; ++i) {
            data[i] /= other.data[i];
        }
        return *this;
    }

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    Vector<T, Dim>& operator+=(U const& other) {
        for (int i = 0; i < Dim; ++i) {
            data[i] += other;
        }
        return *this;
    }

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    Vector<T, Dim>& operator-=(U const& other) {
        for (int i = 0; i < Dim; ++i) {
            data[i] -= other;
        }
        return *this;
    }

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    Vector<T, Dim>& operator*=(U const& other) {
        for (int i = 0; i < Dim; ++i) {
            data[i] *= other;
        }
        return *this;
    }

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    Vector<T, Dim>& operator/=(U const& other) {
        for (int i = 0; i < Dim; ++i) {
            data[i] /= other;
        }
        return *this;
    }

    template <typename U>
    Vector<T, Dim> operator+(U const& other) const {
        auto ret = *this;
        ret += other;
        return ret;
    }

    template <typename U>
    Vector<T, Dim> operator-(U const& other) const {
        auto ret = *this;
        ret -= other;
        return ret;
    }

    template <typename U>
    Vector<T, Dim> operator*(U const& other) const {
        auto ret = *this;
        ret *= other;
        return ret;
    }

    template <typename U>
    Vector<T, Dim> operator/(U const& other) const {
        auto ret = *this;
        ret /= other;
        return ret;
    }
};

template <typename T, int Dim>
inline bool operator==(Vector<T, Dim> const& a, Vector<T, Dim> const& b) {
    for (int i = 0; i < Dim; ++i) {
        if (a.data[i] != b.data[i]) {
            return false;
        }
    }
    return true;
}

template <typename T, int Dim>
inline bool operator!=(Vector<T, Dim> const& a, Vector<T, Dim> const& b) {
    return !(a == b);
}

using FloatVec2 = Vector<float, 2>;
using IntVec2 = Vector<int, 2>;

#pragma pack(pop)


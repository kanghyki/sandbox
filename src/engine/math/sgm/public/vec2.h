#ifndef INCLUDED_VEC2_H
#define INCLUDED_VEC2_H

#include "vector.h"

namespace sgm {
template <typename T> struct vec<2, T> {
    typedef vec<2, T> type;

    union {
        struct {
            T x, y;
        };
        struct {
            T r, g;
        };
    };

    vec() : x(T(0)), y(T(0)) {}

    vec(T const& x) : x(x), y(x) {}

    vec(type const& v) : x(v.x), y(v.y) {}

    vec(T const& x, T const& y) : x(x), y(y) {}

    size_t constexpr length() const { return 2; }

    T& operator[](size_t const& i) {
        assert(i >= 0 && i < length());

        switch (i) {
        case 0:
            return this->x;
        case 1:
            return this->y;
        default:
            return this->x;
        }
    }

    T const& operator[](size_t const& i) const {
        assert(i >= 0 && i < length());

        switch (i) {
        case 0:
            return this->x;
        default:
            return this->y;
        }
    }

    type operator+(type const& v) const { return type{this->x + v.x, this->y + v.y}; }

    type operator-(type const& v) const { return type{this->x - v.x, this->y - v.y}; }

    type operator*(type const& v) const { return type{this->x * v.x, this->y * v.y}; }
};

template <typename T> vec<2, T> operator*(vec<2, T> const& v, T const& scalar) {
    return vec<2, T>{v.x * scalar, v.y * scalar};
}

template <typename T> vec<2, T> operator*(T const& scalar, vec<2, T> const& v) {
    return vec<2, T>{v.x * scalar, v.y * scalar};
}

template <typename T> vec<2, T> operator/(vec<2, T> const& v, T const& scalar) {
    return vec<2, T>{v.x / scalar, v.y / scalar};
}

template <typename T> vec<2, T> operator/(T const& scalar, vec<2, T> const& v) {
    return vec<2, T>{v.x / scalar, v.y / scalar};
}
}; // namespace sgm

#endif

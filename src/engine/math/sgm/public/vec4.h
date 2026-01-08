#ifndef INCLUDED_VEC4_H
#define INCLUDED_VEC4_H

#include "vector.h"

namespace sgm {
template <typename T> struct vec<4, T> {
    typedef vec<4, T> type;

    union {
        struct {
            T x, y, z, w;
        };
        struct {
            T r, g, b, a;
        };
    };

    vec() : x(T(0)), y(T(0)), z(T(0)), w(T(0)) {}

    vec(type const& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    vec(T const& x) : x(x), y(x), z(x), w(x) {}

    vec(T const& x, T const& y, T const& z, T const& w) : x(x), y(y), z(z), w(w) {}

    vec(vec<2, T> const& v, T const& z, T const& w) : x(v.x), y(v.y), z(z), w(w) {}

    vec(vec<3, T> const& v, T const& w) : x(v.x), y(v.y), z(v.z), w(w) {}

    size_t constexpr length() const { return 4; }

    T& operator[](size_t const& i) {
        assert(i >= 0 && i < length());

        switch (i) {
        case 0:
            return this->x;
        case 1:
            return this->y;
        case 2:
            return this->z;
        case 3:
            return this->w;
        default:
            return this->x;
        }
    }

    T const& operator[](size_t const& i) const {
        assert(i >= 0 && i < length());

        switch (i) {
        case 0:
            return this->x;
        case 1:
            return this->y;
        case 2:
            return this->z;
        default:
            return this->w;
        }
    }

    type operator+(type const& v) const {
        return type{this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w};
    }

    type operator-(type const& v) const {
        return type{this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w};
    }

    type operator*(type const& v) const {
        return type{this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w};
    }
};

template <typename T> vec<4, T> operator*(vec<4, T> const& v, T const& scalar) {
    return vec<4, T>{v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar};
}

template <typename T> vec<4, T> operator*(T const& scalar, vec<4, T> const& v) {
    return v * scalar;
}

template <typename T> vec<4, T> operator/(vec<4, T> const& v, T const& scalar) {
    return vec<4, T>{v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar};
}

template <typename T> vec<4, T> operator/(T const& scalar, vec<4, T> const& v) {
    return v / scalar;
}
}; // namespace sgm

#endif

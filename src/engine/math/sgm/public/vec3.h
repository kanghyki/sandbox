#ifndef INCLUDED_VEC3_H
#define INCLUDED_VEC3_H

#include "vector.h"

namespace sgm {
template <typename T> struct vec<3, T> {
    typedef vec<3, T> type;

    union {
        struct {
            T x, y, z;
        };
        struct {
            T r, g, b;
        };
    };

    vec() : x(T(0)), y(T(0)), z(T(0)) {}

    vec(type const& v) : x(v.x), y(v.y), z(v.z) {}

    vec(T const& x) : x(x), y(x), z(x) {}

    vec(T const& x, T const& y, T const& z) : x(x), y(y), z(z) {}

    vec(vec<2, T> const& v, T const& z) : x(v.x), y(v.y), z(z) {}

    size_t constexpr length() const { return 3; }

    T& operator[](size_t const& i) {
        assert(i >= 0 && i < length());

        switch (i) {
        case 0:
            return this->x;
        case 1:
            return this->y;
        case 2:
            return this->z;
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
        default:
            return this->z;
        }
    }

    type operator+(type const& v) const {
        return type{this->x + v.x, this->y + v.y, this->z + v.z};
    }

    type operator-(type const& v) const {
        return type{this->x - v.x, this->y - v.y, this->z - v.z};
    }

    type operator*(type const& v) const {
        return type{this->x * v.x, this->y * v.y, this->z * v.z};
    }
};

template <typename T> vec<3, T> operator*(vec<3, T> const& v, T const& scalar) {
    return vec<3, T>{v.x * scalar, v.y * scalar, v.z * scalar};
}

template <typename T> vec<3, T> operator*(T const& scalar, vec<3, T> const& v) {
    return vec<3, T>{v.x * scalar, v.y * scalar, v.z * scalar};
}

template <typename T> vec<3, T> operator/(vec<3, T> const& v, T const& scalar) {
    return vec<3, T>{v.x / scalar, v.y / scalar, v.z / scalar};
}

template <typename T> vec<3, T> operator/(T const& scalar, vec<3, T> const& v) {
    return vec<3, T>{v.x / scalar, v.y / scalar, v.z / scalar};
}
}; // namespace sgm

#endif

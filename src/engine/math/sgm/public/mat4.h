#ifndef INCLUDED_MAT4_H
#define INCLUDED_MAT4_H

#include "matrix.h"

#include <assert.h>

namespace sgm {
template <typename T> struct mat<4, 4, T> {
    typedef mat<4, 4, T> type;
    typedef vec<4, T> col_type;

    col_type value[4];

    mat()
        : value{{T(0), T(0), T(0), T(0)},
                {T(0), T(0), T(0), T(0)},
                {T(0), T(0), T(0), T(0)},
                {T(0), T(0), T(0), T(0)}} {}

    mat(T v)
        : value{{v, T(0), T(0), T(0)},
                {T(0), v, T(0), T(0)},
                {T(0), T(0), v, T(0)},
                {T(0), T(0), T(0), v}} {}

    mat(type const& m) : value{m[0], m[1], m[2], m[3]} {}

    mat(col_type const& v0, col_type const& v1, col_type const& v2, col_type const& v3)
        : value{v0, v1, v2, v3} {}

    col_type& operator[](size_t i) {
        assert(i >= 0 && i < 4);

        switch (i) {
        case 0:
            return value[0];
        case 1:
            return value[1];
        case 2:
            return value[2];
        case 3:
            return value[3];
        default:
            return value[0];
        }
    }

    col_type const& operator[](size_t i) const {
        assert(i >= 0 && i < 4);

        switch (i) {
        case 0:
            return value[0];
        case 1:
            return value[1];
        case 2:
            return value[2];
        case 3:
            return value[3];
        default:
            return value[0];
        }
    }

    type operator*(type const& m) const {
        type ret;

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    ret[j][i] += value[k][i] * m[j][k];
                }
            }
        }

        return ret;
    }
};

template <typename T> vec<4, T> operator*(mat<4, 4, T> const& mat, vec<4, T> const& v) {
    vec<4, T> ret;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            ret[i] += mat[j][i] * v[j];
        }
    }

    return ret;
}

template <typename T> vec<4, T> operator*(vec<4, T> const& v, mat<4, 4, T> const& mat) {
    vec<4, T> ret;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            ret[j] += v[i] * mat[j][i];
        }
    }

    return ret;
}

}; // namespace sgm

#endif

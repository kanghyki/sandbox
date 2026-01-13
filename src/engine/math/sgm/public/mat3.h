#ifndef INCLUDED_MAT3_H
#define INCLUDED_MAT3_H

#include "matrix.h"

#include <assert.h>

namespace sgm {
template <typename T> struct mat<3, 3, T> {
    typedef mat<3, 3, T> type;
    typedef vec<3, T> col_type;

    col_type value[3];

    mat() : value{{T(0), T(0), T(0)}, {T(0), T(0), T(0)}, {T(0), T(0), T(0)}} {}

    mat(T v) : value{{v, T(0), T(0)}, {T(0), v, T(0)}, {T(0), T(0), v}} {}

    mat(type const& m) : value{m[0], m[1], m[2]} {}

    mat(col_type const& v0, col_type const& v1, col_type const& v2) : value{v0, v1, v2} {}

    col_type& operator[](size_t i) {
        assert(i >= 0 && i < 3);

        switch (i) {
        case 0:
            return value[0];
        case 1:
            return value[1];
        case 2:
            return value[2];
        default:
            return value[0];
        }
    }

    col_type const& operator[](size_t i) const {
        assert(i >= 0 && i < 3);

        switch (i) {
        case 0:
            return value[0];
        case 1:
            return value[1];
        case 2:
            return value[2];
        default:
            return value[0];
        }
    }

    type operator*(type const& m) const {
        type ret;

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                for (int k = 0; k < 3; ++k) {
                    ret[j][i] += value[k][i] * m[j][k];
                }
            }
        }

        return ret;
    }
};

template <typename T> vec<3, T> operator*(mat<3, 3, T> const& mat, vec<3, T> const& v) {
    vec<3, T> ret;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            ret[i] += mat[j][i] * v[j];
        }
    }

    return ret;
}

template <typename T> vec<3, T> operator*(vec<3, T> const& v, mat<3, 3, T> const& mat) {
    vec<3, T> ret;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            ret[j] += v[i] * mat[j][i];
        }
    }

    return ret;
}

}; // namespace sgm

#endif

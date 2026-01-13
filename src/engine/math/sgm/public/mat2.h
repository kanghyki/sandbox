#ifndef INCLUDED_MAT2_H
#define INCLUDED_MAT2_H

#include "matrix.h"

#include <assert.h>

namespace sgm {
template <typename T> struct mat<2, 2, T> {
    typedef mat<2, 2, T> type;
    typedef vec<2, T> col_type;

    col_type value[2];

    mat() : value{{T(0), T(0)}, {T(0), T(0)}} {}

    mat(T v) : value{{v, T(0)}, {T(0), v}} {}

    mat(type const& m) : value{m[0], m[1]} {}

    mat(col_type const& v0, col_type const& v1) : value{v0, v1} {}

    col_type& operator[](size_t i) {
        assert(i >= 0 && i < 2);

        switch (i) {
        case 0:
            return value[0];
        case 1:
            return value[1];
        default:
            return value[0];
        }
    }

    col_type const& operator[](size_t i) const {
        assert(i >= 0 && i < 2);

        switch (i) {
        case 0:
            return value[0];
        case 1:
            return value[1];
        default:
            return value[0];
        }
    }

    type operator*(type const& m) const {
        type ret;

        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                for (int k = 0; k < 2; ++k) {
                    ret[j][i] += value[k][i] * m[j][k];
                }
            }
        }

        return ret;
    }
};

template <typename T> vec<2, T> operator*(mat<2, 2, T> const& mat, vec<2, T> const& v) {
    vec<2, T> ret;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            ret[i] += mat[j][i] * v[j];
        }
    }

    return ret;
}

template <typename T> vec<2, T> operator*(vec<2, T> const& v, mat<2, 2, T> const& mat) {
    vec<2, T> ret;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            ret[j] += v[i] * mat[j][i];
        }
    }

    return ret;
}

}; // namespace sgm

#endif

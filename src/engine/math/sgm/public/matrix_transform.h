#ifndef INCLUDED_MATRIX_TRANSFORM_H
#define INCLUDED_MATRIX_TRANSFORM_H

#include "matrix.h"
#include "trigonometry.h"
#include "vector.h"

namespace sgm {
template <typename T> mat<4, 4, T> translate(mat<4, 4, T> const& m, vec<3, T> const& v) {
    mat<4, 4, T> ret(m);

    ret[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];

    return ret;
}

template <typename T> mat<4, 4, T> scale(mat<4, 4, T> const& m, vec<3, T> const& v) {
    mat<4, 4, T> ret;

    ret[0] = m[0] * v[0];
    ret[1] = m[1] * v[1];
    ret[2] = m[2] * v[2];
    ret[3] = m[3];

    return ret;
}

template <typename T> mat<4, 4, T> rotate(mat<4, 4, T> const& m, T angle, vec<3, T> const& axis) {
    T const ca = cos(angle);
    T const sa = sin(angle);
    vec<3, T> naxis = normalize(axis);
    vec<3, T> temp = (T(1) - ca) * naxis;

    mat<4, 4, T> rot;
    rot[0][0] = ca + temp[0] * naxis[0];
    rot[0][1] = temp[0] * naxis[1] + sa * naxis[2];
    rot[0][2] = temp[0] * naxis[2] - sa * naxis[1];

    rot[1][0] = temp[1] * naxis[0] - sa * naxis[2];
    rot[1][1] = ca + temp[1] * naxis[1];
    rot[1][2] = temp[1] * naxis[2] + sa * naxis[0];

    rot[2][0] = temp[2] * naxis[0] + sa * naxis[1];
    rot[2][1] = temp[2] * naxis[1] - sa * naxis[0];
    rot[2][2] = ca + temp[2] * naxis[2];

    mat<4, 4, T> ret;
    ret[0] = m[0] * rot[0][0] + m[1] * rot[0][1] + m[2] * rot[0][2];
    ret[1] = m[0] * rot[1][0] + m[1] * rot[1][1] + m[2] * rot[1][2];
    ret[2] = m[0] * rot[2][0] + m[1] * rot[2][1] + m[2] * rot[2][2];
    ret[3] = m[3];

    return ret;
}

template <typename T>
mat<4, 4, T> lookAt(vec<3, T> const& eye, vec<3, T> const& center, vec<3, T> const& up) {
    auto za = normalize(center - eye);
    auto xa = normalize(cross(up, za));
    auto ya = cross(za, xa);

    mat<4, 4, T> ret;
    for (int i = 0; i < 3; ++i) {
        ret[i][0] = xa[i];
        ret[i][1] = ya[i];
        ret[i][2] = za[i];
        ret[i][3] = T(0);
    }
    ret[3][0] = dot(-eye, xa);
    ret[3][1] = dot(-eye, ya);
    ret[3][2] = dot(-eye, za);
    ret[3][3] = T(1);

    return ret;
}

template <typename T> mat<4, 4, T> perspective(T fovy, T aspect, T z_near, T z_far) {
    T inv_aspect = T(1) / aspect;
    T d = T(1) / tan(fovy * T(0.5));

    T inv_nf = T(1) / (z_near - z_far);
    T k = (z_near + z_far) * inv_nf;
    T l = T(2) * z_near * z_far * inv_nf;
    return {{inv_aspect * d, T(0), T(0), T(0)},
            {T(0), d, T(0), T(0)},
            {T(0), T(0), k, T(-1)},
            {T(0), T(0), l, T(0)}};
}

}; // namespace sgm

#endif

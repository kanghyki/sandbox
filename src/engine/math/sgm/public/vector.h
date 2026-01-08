#ifndef INCLUDED_VECTOR_H
#define INCLUDED_VECTOR_H

#include <assert.h>
#include <cmath>

namespace sgm {
template <size_t S, typename T> struct vec;

typedef vec<2, float> vec2;
typedef vec<3, float> vec3;
typedef vec<4, float> vec4;

template <size_t S, typename T> vec<S, T> operator-(vec<S, T> const& v) {
    vec<S, T> ret;

    for (size_t i = 0; i < v.length(); ++i) {
        ret[i] = -v[i];
    }

    return ret;
}

template <size_t S, typename T> T length(vec<S, T> const& v) {
    T sum = 0;

    for (size_t i = 0; i < v.length(); ++i) {
        sum += v[i] * v[i];
    }

    return sqrt(sum);
}

template <size_t S, typename T> vec<S, T> normalize(vec<S, T> const& v) {
    vec<S, T> n = v;
    T len = length(n);

    if (len <= 0)
        return n;
    for (size_t i = 0; i < n.length(); ++i) {
        n[i] /= len;
    }

    return n;
}

template <size_t S, typename T> T dot(vec<S, T> const& x, vec<S, T> const& y) {
    T ret = 0;

    for (size_t i = 0; i < S; ++i) {
        ret += x[i] * y[i];
    }

    return ret;
}

template <typename T> vec<3, T> cross(vec<3, T> const& x, vec<3, T> const& y) {
    return vec<3, T>(x[1] * y[2] - x[2] * y[1], x[2] * y[0] - x[0] * y[2],
                     x[0] * y[1] - x[1] * y[0]);
}

template <size_t S, typename T> const T* value_ptr(const vec<S, T>& v) { return &(v.x); }

} // namespace sgm

#include <ostream>
#define VEC_UNION_CHAR "xyzw"

template <size_t S, typename T>
std::ostream& operator<<(std::ostream& stream, sgm::vec<S, T> const& v) {
    stream << "vec" << v.length() << " {";
    for (size_t i = 0; i < v.length(); ++i) {
        stream << " " << VEC_UNION_CHAR[i] << ":" << v[i];
    }
    stream << " }";

    return stream;
}

#endif

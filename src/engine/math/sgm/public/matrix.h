#ifndef INCLUDED_MATRIX_H
#define INCLUDED_MATRIX_H

#include "vector.h"

namespace sgm {
template <size_t R, size_t C, typename T> struct mat;

typedef mat<2, 2, float> mat2;
typedef mat<3, 3, float> mat3;
typedef mat<4, 4, float> mat4;

template <size_t R, size_t C, typename T> const T* value_ptr(const mat<R, C, T>& m) {
    return &(m[0].x);
}
}; // namespace sgm

#include <iomanip>
#include <iostream>
#define VEC_UNION_CHAR "xyzw"

template <size_t R, size_t C, typename T>
std::ostream& operator<<(std::ostream& stream, sgm::mat<R, C, T> const& m) {
    std::cout.precision(5);
    std::cout << std::fixed;
    stream << "mat" << C << "x" << R << " =\n";
    for (size_t i = 0; i < C; ++i) {
        stream << "[";
        for (size_t j = 0; j < R; ++j) {
            stream << " " << i << '.' << VEC_UNION_CHAR[j] << ":" << std::setw(5) << m[i][j];
        }
        stream << " ]\n";
    }

    return stream;
}

#endif

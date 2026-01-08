#ifndef INCLUDED_MATRIX_FUNC_H
#define INCLUDED_MATRIX_FUNC_H

#include "matrix.h"

namespace sgm {
template <size_t S, typename T> mat<S, S, T> transpose(mat<S, S, T> const& mat) {
    sgm::mat<S, S, T> ret;
    for (int i = 0; i < S; ++i) {
        for (int j = 0; j < S; ++j) {
            ret[i][j] = mat[j][i];
        }
    }
    return ret;
}
}; // namespace sgm

#endif

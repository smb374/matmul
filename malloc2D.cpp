//
// Created by poyehchen on 5/16/25.
//

#include <cstdlib>
#include "malloc2D.h"


double **malloc2D(const int row, const int col) {
    const auto mat = static_cast<double **>(malloc(row * sizeof(double *) + row * col * sizeof(double)));
    mat[0] = reinterpret_cast<double *>(mat) + row;

    for (int j = 1; j < row; j++) {
        mat[j] = mat[j - 1] + col;
    }
    return mat;
}

// NOTE: Use on pointer allocated by malloc2D
void free2D(double **p) {
    free(p);
}

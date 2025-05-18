//
// Created by poyehchen on 5/16/25.
//

#include "basic_op.h"

#include <utility>

#define GRID_R 16
#define GRID_C 16

double inner_product(const double* x, const double* y, const int size) {
    double z = 0;
    for (int i = 0; i < size; i++) {
        z += x[i] * y[i];
    }
    return z;
}

inline double inner_product_inline(const double* x, const double* y, const int size) {
    double z = 0;
    for (int i = 0; i < size; i++) {
        z += x[i] * y[i];
    }
    return z;
}

inline double inner_product_full_gridc(const double* x, const double* y) {
    double z = 0;
    for (int i = 0; i < GRID_C; i++) {
        z += x[i] * y[i];
    }
    return z;
}

void matvec_single_grid(double* local_y, const double* local_A, const double* local_x, const int rs, const int rsize,
                        const int cs, const int csize, const int Acol) {
    const double* x_part = local_x + cs;
    for (int i = 0; i < rsize; i++) {
        const int idx = rs + i;
        local_y[idx] += inner_product_inline(&local_A[idx * Acol + cs], x_part, csize);
    }
}

inline void matvec_single_full_grid(double* local_y, const double* local_A, const double* local_x, const int rs,
                                    const int cs, const int Acol) {
    const double* x_part = local_x + cs;
    for (int i = 0; i < GRID_R; i++) {
        const int idx = rs + i;
        local_y[idx] += inner_product_full_gridc(&local_A[idx * Acol + cs], x_part);
    }
}

inline void matvec_single_full_row(double* local_y, const double* local_A, const double* local_x, const int rs,
                                   const int cs, const int csize, const int Acol) {
    const double* x_part = local_x + cs;
    for (int i = 0; i < GRID_R; i++) {
        const int idx = rs + i;
        local_y[idx] += inner_product_inline(&local_A[idx * Acol + cs], x_part, csize);
    }
}

inline void matvec_single_full_col(double* local_y, const double* local_A, const double* local_x, const int rs,
                                   const int rsize, const int cs, const int Acol) {
    const double* x_part = local_x + cs;
    for (int i = 0; i < rsize; i++) {
        const int idx = rs + i;
        local_y[idx] += inner_product_full_gridc(&local_A[idx * Acol + cs], x_part);
    }
}

void matvec_grids(double* local_y, const double* local_A, const double* local_x, const int Ar, const int Ac) {
    const int rsteps = Ar / GRID_R, csteps = Ac / GRID_C;
    const int rrem = Ar % GRID_R, crem = Ac % GRID_C;

    // Full tiles
    for (int i = 0, rs = 0; i < rsteps; i++, rs += GRID_R) {
        for (int j = 0, cs = 0; j < csteps; j++, cs += GRID_C) {
            matvec_single_full_grid(local_y, local_A, local_x, rs, cs, Ac);
        }
    }
    // Tiles with full row
    if (crem) {
        for (int i = 0, rs = 0; i < rsteps; i++, rs += GRID_R) {
            matvec_single_full_row(local_y, local_A, local_x, rs, csteps * GRID_C, crem, Ac);
        }
    }
    // Tiles with full col
    if (rrem) {
        for (int j = 0, cs = 0; j < csteps; j++, cs += GRID_C) {
            matvec_single_full_col(local_y, local_A, local_x, rsteps * GRID_R, rrem, cs, Ac);
        }
    }
    // Residual tile
    if (crem && rrem) {
        matvec_single_grid(local_y, local_A, local_x, rsteps * GRID_R, rrem, csteps * GRID_C, crem, Ac);
    }
}
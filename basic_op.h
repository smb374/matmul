//
// Created by poyehchen on 5/16/25.
//

#ifndef BASIC_OP_H
#define BASIC_OP_H
#include <utility>

double inner_product(const double* x, const double* y, int size);
void matvec_single_grid(double* local_y, const double* local_A, const double* x, int rs, int rsize, int cs, int csize,
                        int Acol);
void matvec_grids(double* local_y, const double* local_A, const double* local_x, int Ar, int Ac);

#endif  // BASIC_OP_H

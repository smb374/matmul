//
// Created by poyehchen on 5/16/25.
//

#ifndef MATRIX_IO_H
#define MATRIX_IO_H

#include <iostream>

double **read_matrix_from_file(const char *file_name, int &row, int &col);

double *read_matrix_from_file_1d(const char *file_name, int &row, int &col);

double *read_vector_from_file(const char *file_name, int &row);

void write_matrix(const double **matrix, int row, int col);

void write_vector(const double *vector, int row);

#endif //MATRIX_IO_H

#include <iostream>
#include <boost/mpl/print.hpp>

#include "matrix_io.h"

int main(const int argc, char* argv[]) {
    if (argc < 2) {
        return 1;
    }
    int row, col;

    const char* filename = argv[1];
    double* vector = read_vector_from_file(filename, row);
    if (!vector) return 0;
    printf("%d\n", row);
    for (int i = 0; i < row; i++) {
        printf("%lf\n", vector[i]);
    }
    free(vector);
    // double** matrix = read_matrix_from_file(filename, row, col);
    //
    // printf("%d %d\n", row, col);
    // for (int i = 0; i < row; i++) {
    //     for (int j = 0; j < col; j++) {
    //         printf("%lf ", matrix[i][j]);
    //     }
    //     printf("\n");
    // }
    //
    // free(matrix);
    return 0;
}
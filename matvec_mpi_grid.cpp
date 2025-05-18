//
// Created by poyehchen on 5/17/25.
//
#include <mpi.h>
#include <pthread.h>

#include <cstring>
#include <vector>

#include "basic_op.h"
#include "matrix_io.h"

void calculate_scatter_param(std::vector<int>& sendcounts, std::vector<int>& displs, const int wrow, int rrow,
                             const int size) {
    sendcounts.resize(size, wrow);
    displs.resize(size, 0);

    for (int i = 1; i < size; i++) {
        if (rrow) {
            sendcounts[i - 1]++;
            rrow--;
        }
        displs[i] = displs[i - 1] + sendcounts[i - 1];
    }
}

double* scatter_A(const double* A, const std::vector<int>& sendcounts, const std::vector<int>& displs, const int col,
                  const int rank) {
    const int row_size = sendcounts[rank];
    auto local_A = new double[row_size * col];

    MPI_Datatype row_vec;
    MPI_Type_contiguous(col, MPI_DOUBLE, &row_vec);
    MPI_Type_commit(&row_vec);

    MPI_Scatterv(A, sendcounts.data(), displs.data(), row_vec, local_A, row_size, row_vec, 0, MPI_COMM_WORLD);

    MPI_Type_free(&row_vec);
    return local_A;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <matrix file> <vector file>\n", argv[0]);
        return 1;
    }
    int rank, size;
    int row, col;
    double *A = nullptr, *x = nullptr, *y = nullptr;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (!rank) {
        int vec_len;
        A = read_matrix_from_file_1d(argv[1], row, col);
        x = read_vector_from_file(argv[2], vec_len);
        if (!x || vec_len != col) {
            delete[] A;
            delete[] x;
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        y = new double[row];
        memset(y, 0, row * sizeof(double));
    }
    const double start = MPI_Wtime();
    // Broadcast A's dimension
    MPI_Bcast(&row, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&col, 1, MPI_INT, 0, MPI_COMM_WORLD);
    auto local_x = new double[col];
    if (!rank) {
        memcpy(local_x, x, col * sizeof(double));
    }
    MPI_Bcast(local_x, col, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    const int wrow = row / size, rrow = row % size;
    // Scatter x to local sizes
    std::vector<int> sendcounts, displs;
    calculate_scatter_param(sendcounts, displs, wrow, rrow, size);
    const auto local_A = scatter_A(A, sendcounts, displs, col, rank);
    const auto local_y = new double[sendcounts[rank]];
    memset(local_y, 0, sendcounts[rank] * sizeof(double));

    matvec_grids(local_y, local_A, local_x, sendcounts[rank], col);

    MPI_Gatherv(local_y, sendcounts[rank], MPI_DOUBLE, y, sendcounts.data(), displs.data(), MPI_DOUBLE, 0,
                MPI_COMM_WORLD);

    // MPI_Reduce(local_y, y, row, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (!rank) {
        const double end = MPI_Wtime();

        fprintf(stderr, "Time elapsed: %lf secs\n", end - start);
        write_vector(y, row);
    }

    if (!rank) {
        delete[] A;
        delete[] x;
        delete[] y;
    }
    delete[] local_A;
    delete[] local_x;
    delete[] local_y;

    MPI_Finalize();
    return 0;
}

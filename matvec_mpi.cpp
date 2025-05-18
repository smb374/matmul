//
// Created by poyehchen on 5/16/25.
//

#include <mpi.h>

#include "basic_op.h"
#include "matrix_io.h"

// Calculate y = Ax
int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <matrix file> <vector file>\n", argv[0]);
        return 1;
    }
    int rank, size;
    int mrow, vlen;
    double* mat = nullptr;
    double *vec = nullptr, *res = nullptr, *lres = nullptr, *lmat = nullptr;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        int mcol;
        // Master reads the matrix and the vector from file.
        const char* mat_file = argv[1];
        const char* vec_file = argv[2];

        mat = read_matrix_from_file_1d(mat_file, mrow, mcol);
        vec = read_vector_from_file(vec_file, vlen);
        if (!vec || mcol != vlen) {
            int tag = -1;
            MPI_Bcast(&tag, 1, MPI_INT, 0, MPI_COMM_WORLD);
            delete[] mat;
            MPI_Finalize();
            return 1;
        }
        // Allocates the result vector.
        res = new double[mrow];
    }
    const double start = MPI_Wtime();
    // Broadcast shared value.
    MPI_Bcast(&mrow, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&vlen, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (vlen == -1) {
        delete[] mat;
        MPI_Finalize();
        return 1;
    };
    // Allocates vec for others to receive.
    if (!vec) vec = new double[vlen];
    MPI_Bcast(vec, vlen, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    const int wrows = mrow / size;
    int rest = mrow % size;
    double* lbuf = new double[wrows + 1 + (wrows + 1) * vlen];
    lres = lbuf;
    lmat = lbuf + (wrows + 1);
    int* sgvec = new int[size * 4];
    int* sendcounts = sgvec;
    int* displs = sendcounts + size;
    int* stride = displs + size;
    int* sdispls = stride + size;

    displs[0] = 0;
    sdispls[0] = 0;
    sendcounts[0] = wrows * vlen;
    stride[0] = wrows;
    for (int i = 1; i < size; i++) {
        if (rest) {
            sendcounts[i - 1] += vlen;
            stride[i - 1] += 1;
            rest--;
        }
        displs[i] = displs[i - 1] + sendcounts[i - 1];
        sdispls[i] = sdispls[i - 1] + stride[i - 1];
        sendcounts[i] = wrows * vlen;
        stride[i] = wrows;
    }

    MPI_Scatterv(mat, sendcounts, displs, MPI_DOUBLE, lmat, (wrows + 1) * vlen, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < stride[rank]; i++) {
        lres[i] = inner_product(&lmat[i * vlen], vec, vlen);
    }

    MPI_Gatherv(lres, stride[rank], MPI_DOUBLE, res, stride, sdispls, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (!rank) {
        const double end = MPI_Wtime();
        fprintf(stderr, "Time elapsed: %lf secs\n", end - start);
        write_vector(res, mrow);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    // Cleanup
    delete[] sgvec;
    delete[] lbuf;
    delete[] vec;
    delete[] res;
    delete[] mat;

    MPI_Finalize();
    return 0;
}

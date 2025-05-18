//
// Created by poyehchen on 5/17/25.
//

#include <chrono>
#include <cstring>
#include <thread>
#include <vector>

#include "basic_op.h"
#include "matrix_io.h"

#define NUM_THREADS 8

double* mat;
double* vec;
double* res;
double* loc_res[NUM_THREADS];
int loc_size[NUM_THREADS];
int row, col, vlen, wrow, wcol, rrow, rcol;

// Row wise grid distribution
void matvec_worker(const int id, const double* local_A, const int rows, const int cols) {
    loc_res[id] = new double[rows];
    loc_size[id] = rows;
    memset(loc_res[id], 0, sizeof(double) * rows);

    matvec_grids(loc_res[id], local_A, vec, rows, cols);
}

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <matrix file> <vector file>\n", argv[0]);
        return 1;
    }

    const char* mat_file = argv[1];
    const char* vec_file = argv[2];

    mat = read_matrix_from_file_1d(mat_file, row, col);
    vec = read_vector_from_file(vec_file, vlen);
    if (!vec || vlen != col) {
        delete[] mat;
        delete[] vec;
        return 1;
    }

    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS - 1);
    res = new double[row];
    memset(res, 0, row * sizeof(double));

    const auto stime{std::chrono::steady_clock::now()};
    if (row * col < 10000) {
        matvec_single_grid(res, mat, vec, 0, row, 0, col, col);
    } else {
        wrow = row / NUM_THREADS;
        wcol = col / NUM_THREADS;
        rrow = row % NUM_THREADS;
        rcol = col % NUM_THREADS;
        int rs = 0;

        for (int i = 0; i < NUM_THREADS - 1; i++) {
            int rsize = wrow;
            if (rrow) {
                rsize++;
                rrow--;
            }
            threads.emplace_back(matvec_worker, i, &mat[rs * col], rsize, col);
            rs += rsize;
        }
        matvec_worker(NUM_THREADS - 1, &mat[rs * col], wrow, col);

        for (auto& t : threads) {
            t.join();
        }

        int base = 0;
        for (int i = 0; i < NUM_THREADS; i++) {
            for (int j = 0; j < loc_size[i]; j++) {
                res[base + j] = loc_res[i][j];
            }

            base += loc_size[i];
        }
    }

    const auto etime{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{etime - stime};

    fprintf(stderr, "Time elapsed: %lf secs\n", elapsed_seconds.count());
    write_vector(res, row);

    delete[] mat;
    delete[] vec;
    delete[] res;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (loc_res[i]) delete[] loc_res[i];
    }
    return 0;
}

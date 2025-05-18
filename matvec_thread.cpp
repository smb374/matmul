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
int row, col, vlen;

void matvec_local(const int start, const int size) {
    for (int i = start; i < start + size; i++) {
        res[i] = inner_product(&mat[i * col], vec, vlen);
    }
}

int main(int argc, char* argv[]) {
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
    res = new double[row];
    memset(res, 0, row * sizeof(double));

    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS - 1);
    const auto stime{std::chrono::steady_clock::now()};

    if (row * col < 10000) {
        matvec_single_grid(res, mat, vec, 0, row, 0, col, col);
    } else {
        int wrow = row / NUM_THREADS;
        int rem = row % NUM_THREADS;
        int start = 0;

        for (int i = 0; i < NUM_THREADS - 1; i++) {
            if (rem) {
                threads.emplace_back(matvec_local, start, wrow + 1);
                start += wrow + 1;
                rem--;
            } else {
                threads.emplace_back(matvec_local, start, wrow);
                start += wrow;
            }
        }

        matvec_local(start, wrow);

        for (auto& t : threads) {
            t.join();
        }
    }
    const auto etime{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{etime - stime};

    fprintf(stderr, "Time elapsed: %lf secs\n", elapsed_seconds.count());
    write_vector(res, row);

    delete[] mat;
    delete[] vec;
    delete[] res;
    return 0;
}
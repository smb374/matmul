//
// Created by poyehchen on 5/16/25.
//

#include "matrix_io.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "malloc2D.h"

double **read_matrix_from_file(const char *file_name, int &row, int &col) {
    std::ifstream f(file_name);
    std::string line;
    std::istringstream ss;

    std::getline(f, line);
    ss.str(line);
    ss >> row >> col;
    ss.clear();

    double **matrix = malloc2D(row, col);

    for (int i = 0; i < row; i++) {
        std::getline(f, line);
        ss.str(line);
        for (int j = 0; j < col; j++) {
            ss >> matrix[i][j];
        }
        ss.clear();
    }
    f.close();

    return matrix;
}

double *read_matrix_from_file_1d(const char *file_name, int &row, int &col) {
    std::ifstream f(file_name);
    std::string line;
    std::istringstream ss;

    std::getline(f, line);
    ss.str(line);
    ss >> row >> col;
    ss.clear();

    auto matrix = new double[row * col];

    for (int i = 0; i < row; i++) {
        std::getline(f, line);
        ss.str(line);
        for (int j = 0; j < col; j++) {
            ss >> matrix[i * col + j];
        }
        ss.clear();
    }
    f.close();

    return matrix;
}

double *read_vector_from_file(const char *file_name, int &row) {
    std::ifstream f(file_name);
    std::string line;
    std::istringstream ss;
    int col;

    std::getline(f, line);
    ss.str(line);
    ss >> row >> col;
    ss.clear();

    if (col != 1) {
        return nullptr;
    }

    auto vector = new double[row];

    for (int i = 0; i < row; i++) {
        std::getline(f, line);
        ss.str(line);
        ss >> vector[i];
        ss.clear();
    }
    f.close();

    return vector;
}

void write_matrix(const double **matrix, int row, int col) {
    std::string buf;
    buf.reserve(65536);
    std::stringstream ss(buf);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            ss << std::setprecision(18) << std::scientific << matrix[i][j];
            ss << (j == col - 1 ? "\n" : " ");
        }
    }
    fputs(ss.str().c_str(), stdout);
}

void write_vector(const double *vector, int row) {
    std::string buf;
    buf.reserve(65536);
    std::stringstream ss(buf);
    for (int i = 0; i < row; i++) {
        ss << std::setprecision(18) << std::scientific << vector[i] << '\n';
    }
    fputs(ss.str().c_str(), stdout);
}

//
// Created by poyehchen on 5/17/25.
//

#include <mpi.h>

#include <iostream>

int main(int argc, char *argv[]) {
    int rank;
    int n = 10;
    int buffer[10];
    int blocklength[2];
    int index[2];
    int i;
    MPI_Datatype newtype;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    blocklength[0] = 1;
    blocklength[1] = 2;
    index[0] = 0;
    index[1] = 3;
    MPI_Type_indexed(2, blocklength, index, MPI_INT, &newtype);
    MPI_Type_commit(&newtype);
    if (rank == 0) {
        for (i = 0; i < n; i++) {
            buffer[i] = i + 1;
        }

        MPI_Send(&buffer, 1, newtype, 1, 99, MPI_COMM_WORLD);
    }

    if (rank == 1) {
        for (i = 0; i < n; i++) {
            buffer[i] = 100;
        }
        MPI_Recv(&buffer, 1, newtype, 0, 99, MPI_COMM_WORLD, &status);
        for (i = 0; i < n; i++) {
            printf("buffer[%d] is %d\n", i, buffer[i]);
        }
    }
    MPI_Finalize();
    return 0;
}
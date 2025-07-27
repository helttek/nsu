#include "util.h"
#include <cstdlib>
#include <iostream>
#include <mpi/mpi.h>
#include <mpich/mpi_proto.h>
#include <stdlib.h>
#include <string>

int main(int argc, char *argv[]) {
  const int n1 = atoi(argv[1]);
  const int n2 = atoi(argv[2]);
  const int n3 = atoi(argv[3]);
  const int p1 = atoi(argv[4]);
  const int p2 = atoi(argv[5]);

  int rank, size, newRank, newSize;

  double start, finish;

  if ((n1 % p1 != 0) || (n3 % p2 != 0)) {
    std::cout << "Error: sizes are not dividible\n";
    return 0;
  }

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size != 16) {
    std::cerr << "Please, use 16 processes.\n";
    return 0;
  }

  float *A = new float[n1 * n2];
  float *B = new float[n2 * n3];
  float *C = new float[n1 * n3];
  float *CBuf = new float[n1 / p1 * n3 / p2];
  float *ABuf = new float[n1 / p1 * n2];
  float *BBuf = new float[n3 / p2 * n2];
  int *dim = new int[2];
  int *period = new int[2];
  int *rankCoords = new int[2];
  int *remain_dimsX = new int[2];
  int *remain_dimsY = new int[2];
  int *displs = new int[p1 * p2];
  int *recvCount = new int[p1 * p2];

  InitMat(A, n1, n2, 1);
  InitMat(B, n2, n3, 1);
  InitMat(C, n1, n3, 0);
  if (rank == 0) {
    // PrintMatrix(A, n1, n2);
    // PrintMatrix(B, n2, n3);
    // PrintMatrix(C, n1, n3);
  }
  InitMat(ABuf, n1 / p1, n2, 0);
  InitMat(BBuf, n2, n3 / p2, 0);
  InitMat(CBuf, n1 / p1, n3 / p2, 0);
  dim[0] = p1;
  dim[1] = p2;
  period[0] = 1;
  period[1] = 1;
  remain_dimsX[0] = true;
  remain_dimsX[1] = false;
  remain_dimsY[0] = false;
  remain_dimsY[1] = true;

  MPI_Comm cartesianGrid;
  MPI_Comm x;
  MPI_Comm y;
  MPI_Cart_create(MPI_COMM_WORLD, 2, dim, period, 0, &cartesianGrid);
  MPI_Cart_sub(cartesianGrid, remain_dimsX, &x);
  MPI_Cart_sub(cartesianGrid, remain_dimsY, &y);
  MPI_Comm_rank(cartesianGrid, &newRank);
  MPI_Cart_coords(cartesianGrid, newRank, 2, rankCoords);

  if (rankCoords[1] == 0) {
    MPI_Scatter(A, n1 / p1 * n2, MPI_FLOAT, ABuf, n1 / p1 * n2, MPI_FLOAT, 0,
                x);
    // PrintMatrix(ABuf, n1 / p1, n2);
  }
  MPI_Bcast(ABuf, n1 / p1 * n2, MPI_FLOAT, 0, y);

  MPI_Datatype columnsNotResized, columnsResized;
  MPI_Type_vector(n2, n3 / p2, n3, MPI_FLOAT, &columnsNotResized);
  MPI_Type_commit(&columnsNotResized);
  MPI_Type_create_resized(columnsNotResized, 0, n3 / p2 * sizeof(float),
                          &columnsResized);
  MPI_Type_commit(&columnsResized);
  if (rankCoords[0] == 0) {
    MPI_Scatter(B, 1, columnsResized, BBuf, n3 / p2 * n2, MPI_FLOAT, 0, y);
    // PrintMatrix(BBuf, n2, n3 / p2);
  }

  MPI_Bcast(BBuf, n3 / p2 * n2, MPI_FLOAT, 0, x);

  start = MPI_Wtime();

  Mul(ABuf, BBuf, CBuf, n1 / p1, n2, n3 / p2);

  finish = MPI_Wtime();

  MPI_Datatype submatNotResized, submatResized;
  MPI_Type_vector(n1 / p1, n3 / p2, n3, MPI_FLOAT, &submatNotResized);
  MPI_Type_commit(&submatNotResized);
  MPI_Type_create_resized(submatNotResized, 0, n3 / p2 * sizeof(float),
                          &submatResized);
  MPI_Type_commit(&submatResized);
  /*
  recvcounts
    non-negative integer array (of length group size) containing the number of
    elements that are received from each process (non-negative integer)
  displs
    integer array (of length group size). Entry i specifies the displacement
    relative to recvbuf at which to place the incoming data from process i
    (integer)
  */
  int submatSize = 0, j;
  for (int i = 0; i < p1; ++i) {
    for (j = 0; j < p2; ++j) {
      recvCount[i * p2 + j] = 1;
      displs[i * p2 + j] = submatSize;
      ++submatSize;
      if (rank == 0) {
        // std::cout << displs[i * p2 + j] << '\n';
      }
    }
    submatSize += j * (n1 / p1 - 1);
  }
  MPI_Gatherv(CBuf, n1 / p1 * n3 / p2, MPI_FLOAT, C, recvCount, displs,
              submatResized, 0, MPI_COMM_WORLD);

  MPI_Type_free(&submatNotResized);
  MPI_Type_free(&submatResized);
  MPI_Type_free(&columnsNotResized);
  MPI_Type_free(&columnsResized);

  if (rank == 0) {
    std::cout
        << "------------------------------------------------------------\n";
    std::cout << "n1: " << n1 << ", n2: " << n2 << ", n3: " << n3
              << ", p1: " << p1 << ", p2: " << p2 << '\n';
    std::cout << "time spent: " << finish - start << '\n';
    // PrintMatrix(C, n1, n3);
    std::cout
        << "------------------------------------------------------------\n";
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] CBuf;
    delete[] dim;
    delete[] period;
    delete[] rankCoords;
    delete[] ABuf;
    delete[] remain_dimsX;
    delete[] remain_dimsY;
    delete[] BBuf;
    delete[] displs;
    delete[] recvCount;
  }
  MPI_Finalize();
  return 0;
}

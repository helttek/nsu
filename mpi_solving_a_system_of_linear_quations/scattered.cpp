#include <mpi/mpi.h>
#include <mpich/mpi_proto.h>

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#define MAIN_PROCESS 0
#define N 2500

void Sub(const float *vec1, const float *vec2, float *res, const int size)
{
  for (int i = 0; i < size; ++i)
  {
    res[i] = vec1[i] - vec2[i];
  }
}

void Mul(const float *mat, const float *vec, float *res, const int N1,
         const int N2)
{
  for (int i = 0; i < N1; ++i)
  {
    res[i] = 0;
    for (int j = 0; j < N2; ++j)
    {
      res[i] += mat[i * N + j] * vec[j];
    }
  }
}

void InitScattervData(int *sendcounts, int *displs, const int numOfProcesses,
                      const int currentProcess)
{
  int baseline = (N / numOfProcesses) * N;
  int rest = N % numOfProcesses;
  int curDispls = 0;
  if (currentProcess == MAIN_PROCESS)
  {
    for (int i = 0; i < numOfProcesses; ++i)
    {
      displs[i] = curDispls;
      sendcounts[i] = baseline;
      if (i < rest)
      {
        sendcounts[i] += N;
      }
      curDispls += sendcounts[i];
    }
  }
}

void PrintVec(const float *vec, const int size)
{
  std::cout << "[";
  for (int i = 0; i < size; ++i)
  {
    std::cout << vec[i] << ' ';
  }
  std::cout << ']';
}

void PrintMat(const float *mat, const int size)
{
  std::cout << "[";
  for (int i = 0; i < size; ++i)
  {
    for (int j = 0; j < size; ++j)
    {
      std::cout << mat[i * N + j] << ' ';
    }
    if (i != size - 1)
    {
      std::cout << '\n';
    }
    else
    {
      std::cout << ']';
    }
  }
}

void readData(std::string filename, const int size, float *buf)
{
  std::ifstream in(filename, std::ios::binary);
  if (!in.is_open())
  {
    throw std::runtime_error("Couldn't open file.\n");
  }
  in.read(reinterpret_cast<char *>(buf), size * sizeof(float));
}

void WriteData(std::string filename, const int size, const float *buf)
{
  std::ofstream out(filename, std::ios::binary);
  if (!out.is_open())
  {
    throw std::runtime_error("Couldn't open file.\n");
  }
  out.write(reinterpret_cast<const char *>(buf), size * sizeof(float));
}

void FillWithValue(float *vec, const float value, const int size)
{
  for (int i = 0; i < size; ++i)
  {
    vec[i] = value;
  }
}

void InitGathervData(int *vecRecvcount, int *vecRecvdispls, int *matSendcounts,
                     int *matDispls, const int numOfProcesses,
                     const int currentProcess)
{
  int curDispls;
  if (currentProcess == MAIN_PROCESS)
  {
    curDispls = 0;
    for (int i = 0; i < numOfProcesses; ++i)
    {
      vecRecvdispls[i] = curDispls;
      vecRecvcount[i] = matSendcounts[i] / N;
      curDispls += vecRecvcount[i];
    }
  }
}

void MulByScal(const float *vec, const float scal, float *res, const int size)
{
  for (int i = 0; i < size; ++i)
  {
    res[i] = vec[i] * scal;
  }
}

float Norm(const float *vec, const int size)
{
  float res = 0;
  for (int i = 0; i < size; ++i)
  {
    res += vec[i] * vec[i];
  }
  return sqrtf(res);
}

int main(int argc, char *argv[])
{
  int numOfProcesses;
  int currentProcess;
  int iterations = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
  MPI_Comm_rank(MPI_COMM_WORLD, &currentProcess);

  int *matSendcounts = new int[numOfProcesses];
  int *matDispls = new int[numOfProcesses];
  int *vecRecvcounts = new int[numOfProcesses];
  int *vecDispls = new int[numOfProcesses];

  InitScattervData(matSendcounts, matDispls, numOfProcesses, currentProcess);
  InitGathervData(vecRecvcounts, vecDispls, matSendcounts, matDispls,
                  numOfProcesses, currentProcess);

  const float tau = -0.01;
  const float epsilon = 0.0001; // 0.00025

  MPI_Bcast(matSendcounts, numOfProcesses, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(matDispls, numOfProcesses, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(vecRecvcounts, numOfProcesses, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(vecDispls, numOfProcesses, MPI_INT, 0, MPI_COMM_WORLD);

  float *A = new float[N * N];
  float *b = new float[N];
  float *xn = new float[N];
  float *Axnb = new float[N];
  float *AxnbBuf = new float[matSendcounts[currentProcess] / N];
  float *Abuf = new float[matSendcounts[currentProcess]];
  float *bBuf = new float[vecRecvcounts[currentProcess]];
  float *xnBuf = new float[vecRecvcounts[currentProcess]];

  float bNorm;
  float AxnbNorm;
  float endClause = epsilon + 1;

  double startTime;
  double finishTime;

  if (currentProcess == MAIN_PROCESS)
  {
    readData("matA.bin", N * N, A);
    readData("vecB.bin", N, b);
  }
  FillWithValue(xn, 0, N);
  FillWithValue(AxnbBuf, 0, matSendcounts[currentProcess] / N);
  FillWithValue(Abuf, 0, matSendcounts[currentProcess]);
  FillWithValue(Axnb, 0, N);
  MPI_Barrier(MPI_COMM_WORLD);

  if (currentProcess == MAIN_PROCESS)
  {
    bNorm = Norm(b, N);
  }
  MPI_Bcast(&bNorm, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Scatterv(A, matSendcounts, matDispls, MPI_FLOAT, Abuf,
               matSendcounts[currentProcess], MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Scatterv(b, vecRecvcounts, vecDispls, MPI_FLOAT, bBuf, vecRecvcounts[currentProcess], MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Bcast(xn, N, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Bcast(b, N, MPI_FLOAT, 0, MPI_COMM_WORLD); // dup variant
  MPI_Scatterv(xn, vecRecvcounts, vecDispls, MPI_FLOAT, xnBuf, vecRecvcounts[currentProcess], MPI_FLOAT, 0, MPI_COMM_WORLD);

  startTime = MPI_Wtime();
  while (endClause >= epsilon)
  {
    Mul(Abuf, xn, AxnbBuf, matSendcounts[currentProcess] / N, N);
    MPI_Scatterv(xn, vecRecvcounts, vecDispls, MPI_FLOAT, xnBuf, vecRecvcounts[currentProcess], MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    Sub(AxnbBuf, bBuf, AxnbBuf, vecRecvcounts[currentProcess]);
    MPI_Barrier(MPI_COMM_WORLD);
    MulByScal(AxnbBuf, tau, AxnbBuf, matSendcounts[currentProcess] / N);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gatherv(AxnbBuf, vecRecvcounts[currentProcess], MPI_FLOAT, Axnb,
                vecRecvcounts, vecDispls, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if (currentProcess == MAIN_PROCESS)
    {
      iterations++;
      AxnbNorm = Norm(Axnb, N);
      endClause = AxnbNorm / bNorm;
      // std::cout << endClause << '\n';
    }
    Sub(xnBuf, AxnbBuf, xnBuf, vecRecvcounts[currentProcess]);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gatherv(xnBuf, vecRecvcounts[currentProcess], MPI_FLOAT, xn,
                vecRecvcounts, vecDispls, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(xn, N, MPI_FLOAT, 0, MPI_COMM_WORLD); // too stupid to determine whether to turn it off or not
    MPI_Bcast(&endClause, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
  }
  finishTime = MPI_Wtime();

  if (currentProcess == MAIN_PROCESS)
  {
    std::cout << '\n';
    std::cout << "number of processes: " << numOfProcesses << '\n';
    std::cout << "time: " << finishTime - startTime << '\n';
    std::cout << "iterations: " << iterations << '\n';
    std::cout << '\n';
    WriteData("vecXN.bin", N, xn);
    std::ofstream out("scat_time_table.csv", std::ios_base::app);
    out << numOfProcesses << ',' << finishTime - startTime << '\n';
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();

  delete[] A;
  delete[] b;
  delete[] xn;
  delete[] Axnb;
  delete[] AxnbBuf;
  delete[] Abuf;
  delete[] matSendcounts;
  delete[] matDispls;
  delete[] vecRecvcounts;
  delete[] vecDispls;
  delete[] bBuf;
  delete[] xnBuf;
  return 0;
}
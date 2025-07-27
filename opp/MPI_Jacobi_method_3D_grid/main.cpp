#include <mpich/mpi.h>
#include <mpich/mpi_proto.h>
#include <iostream>
#include <limits>
#include <cstring>
#include <cstdint>

#define D 2
#define N 384
// #define x0 -1
// #define y0 -1
// #define z0 -1
const float x0 = -1.0f;
const float y0 = -1.0f;
const float z0 = -1.0f;
// #define h D / (N - 1)
const float h = (float)(((float)D) / ((float)N - 1.0f));

float Phi(const float x, const float y, const float z)
{
    return x * x + y * y + z * z;
}

float Rho(const float x, const float y, const float z, const float a)
{
    return 6.f - a * Phi(x, y, z);
}

float Max(const float *a, const float *b, const int height, const int size)
{
    float max = 0;
    for (int k = 0; k < height; k++)
    {
        for (int j = 0; j < size; j++)
        {
            for (int i = 0; i < size; i++)
            {
                if (max < std::abs(a[k * size * size + j * size + i] - b[k * size * size + j * size + i]))
                {
                    max = std::abs(a[k * size * size + j * size + i] - b[k * size * size + j * size + i]);
                }
            }
        }
    }
    return max;
}

float GetAbsCoords(const int dim, const int i)
{
    switch (dim)
    {
    case 1:
        return (float)x0 + (float)i * (float)h;
    case 2:
        return (float)y0 + (float)i * (float)h;
    case 3:
        return (float)z0 + (float)i * (float)h;
    default:
        std::cout << "BAD!!!!\n";
        return -100;
    }
}

void InitGridBuf(float *a, const int height, const int rank, const int numberOfRanks)
{
    for (int i = 0; i < height; i++)
    {
        int absI = rank * (height - 2) + i - 1;
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                if (k == 0 || k == N - 1 || j == 0 || j == N - 1 || absI == 0 || absI == N - 1)
                {
                    a[i * N * N + j * N + k] = Phi(GetAbsCoords(1, k), GetAbsCoords(2, j), GetAbsCoords(3, absI));
                }
                else
                {
                    a[i * N * N + j * N + k] = 0.f;
                }
            }
        }
    }
}

void PrintRectPrism(const float *a, const int n, const int height)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < n; k++)
            {
                std::cout << a[i * n * n + j * n + k] << ' ';
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    }
}

float CalcNextGridBuf(const int index, float *gridBuf, float *gridBufNext, const int rank, const int numberOfRanks, const float a)
{
    int absIndex = rank * N / numberOfRanks + index - 1;
    float res = std::numeric_limits<float>::min();
    if (absIndex != 0 && absIndex != N - 1)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (i != 0 && i != N - 1 && j != 0 && j != N - 1)
                {
                    gridBufNext[N * N * index + N * i + j] = ((gridBuf[N * N * (index + 1) + N * i + j] +
                                                               gridBuf[N * N * (index - 1) + N * i + j] +
                                                               gridBuf[N * N * index + N * (i + 1) + j] +
                                                               gridBuf[N * N * index + N * (i - 1) + j] +
                                                               gridBuf[N * N * index + N * i + j + 1] +
                                                               gridBuf[N * N * index + N * i + j - 1]) /
                                                                  ((float)(h * h)) -
                                                              Rho(GetAbsCoords(1, j), GetAbsCoords(2, i), GetAbsCoords(3, absIndex), a)) /
                                                             (6.0f / ((float)((float)h * (float)h)) + a);
                }
                else
                {
                    gridBufNext[N * N * index + N * i + j] = gridBuf[N * N * index + N * i + j];
                }
                res = std::max(res, std::abs(gridBufNext[N * N * index + N * i + j] - gridBuf[N * N * index + N * i + j]));
            }
        }
        return res;
    }
    return 0.f;
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    const float a = 1e5;
    const float epsilon = 1e-8;
    int rank;
    int size;
    float max = std::numeric_limits<float>::min();
    float gridMax = std::numeric_limits<float>::min();
    float gridBufMax = std::numeric_limits<float>::min();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0 && rank == 0)
    {
        std::cerr << "Number of nodes is not dividible by the amount of ranks.\n";
        return 0;
    }

    float *grid = new float[N * N * N];
    float *gridBuf = new float[N * N * (N / size + 2)];
    float *gridBufNext = new float[N * N * (N / size + 2)];

    InitGridBuf(gridBuf, N / size + 2, rank, size);
    if (rank == 0)
    {
        memcpy(gridBufNext + N * N, gridBuf + N * N, N * N * sizeof(float));
    }
    if (rank == size - 1)
    {
        memcpy(gridBufNext + N * N * N / size, gridBuf + N * N * N / size, N * N * sizeof(float));
    }

    MPI_Request bottomSend;
    MPI_Request bottomRecv;
    MPI_Request topSend;
    MPI_Request topRecv;

    int iterations = 0;

    double start = MPI_Wtime();

    do
    {
        if (rank == 0)
        {
            ++iterations;
        }

        gridMax = std::numeric_limits<float>::min();
        if (rank > 0)
        {
            MPI_Isend(gridBuf + N * N, N * N, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD, &bottomSend);
            MPI_Irecv(gridBuf, N * N, MPI_FLOAT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &bottomRecv);
        }
        if (rank < size - 1)
        {
            MPI_Isend(gridBuf + N * N * N / size, N * N, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD, &topSend);
            MPI_Irecv(gridBuf + N * N * (N / size + 1), N * N, MPI_FLOAT, rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &topRecv);
        }
        for (int i = 2; i < N / size; i++)
        {
            gridBufMax = CalcNextGridBuf(i, gridBuf, gridBufNext, rank, size, a);
            gridMax = std::max(gridBufMax, gridMax);
        }

        if (rank > 0)
        {
            MPI_Wait(&bottomRecv, MPI_STATUS_IGNORE);
        }
        gridBufMax = CalcNextGridBuf(1, gridBuf, gridBufNext, rank, size, a);
        gridMax = std::max(gridBufMax, gridMax);
        if (rank < size - 1)
        {
            MPI_Wait(&topRecv, MPI_STATUS_IGNORE);
        }
        gridBufMax = CalcNextGridBuf(N / size, gridBuf, gridBufNext, rank, size, a);
        gridMax = std::max(gridBufMax, gridMax);
        if (rank > 0)
        {
            MPI_Wait(&bottomSend, MPI_STATUS_IGNORE);
        }
        if (rank < size - 1)
        {
            MPI_Wait(&topSend, MPI_STATUS_IGNORE);
        }
        // std::memcpy(gridBuf, gridBufNext, (N / size + 2) * N * N * sizeof(float));
        float *tmp = gridBuf;
        gridBuf = gridBufNext;
        gridBufNext = tmp;

        MPI_Allreduce(&gridMax, &max, 1, MPI_FLOAT, MPI_MAX, MPI_COMM_WORLD);
        // if (rank == 0)
        // {
        //     std::cout << "max is: " << max << '\n';
        // }

    } while (max >= epsilon);

    double finish = MPI_Wtime();

    MPI_Gather(gridBuf + N * N, N * N * N / size, MPI_FLOAT, grid, N * N * N / size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        // PrintRectPrism(grid, N, N);
        std::cout << "--------------------------------------------------\n";
        std::cout << "time: " << finish - start << '\n';
        std::cout << "tierations: " << iterations << '\n';
        std::cout << "--------------------------------------------------\n";
    }

    MPI_Finalize();

    delete[] grid;
    delete[] gridBuf;
    delete[] gridBufNext;

    return 0;
}

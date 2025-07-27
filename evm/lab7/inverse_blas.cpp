#include <cstdlib>
#include <cblas.h>
#include <cblas-openblas.h>
#include "inverse_blas.h"

float* NewMatrix3(int N) {
    auto* A = new float [N * N];
    return A;
}

void DeleteMatrix3(float* A) {
    operator delete [](A);
}

static float* Transpose(const float* A, float* AT, const int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            AT[i * N + j] = A[j * N + i];
        }
    }
    return AT;
}

static float CalcA1(const float* A, const int N) {
    float max = 0, sum = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sum += A[i * N + j];
        }
        if (sum > max) {
            max = sum;
        }
        sum = 0;
    }
    return max;
}


static float CalcAInf(const float* A, const int N) {
    float max = 0, sum = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sum += A[j * N + i];
        }
        if (sum > max) {
            max = sum;
        }
        sum = 0;
    }
    return max;
}

float* NewI3(const int N) {
    float* I = NewMatrix3(N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            I[i * N + j] = (i == j) ? 1 : 0;
        }
    }
    return I;
}

float* InvertMatrix3(const float* A, const int N) {
    float* sum = NewI3(N);
    float* I = NewI3(N);
    float* B = NewMatrix3(N);
    cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, N, N, N,
                1 / CalcA1(A, N) * CalcAInf(A, N), A, N, I, N, 0, B, N);
    float* BA = NewMatrix3(N);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0,
                A, N, B, N, 0, BA, N);
    float* R = NewMatrix3(N);
    float* BAneg = NewMatrix3(N);
    cblas_saxpy(N * N, -1.0, BA, 1, BAneg, 1);
    cblas_scopy(N * N, BAneg, 1, R, 1);
    DeleteMatrix3(BA);
    DeleteMatrix3(BAneg);
    float* R_prev = NewMatrix3(N);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0,
                R, N, R, N, 0, R_prev, N);
    float* R_cur = NewI3(N);
    cblas_saxpy(N * N, 1.0, R_prev, 1, sum, 1);
    for (int i = 2; i < 10; i++) {
        if (i % 2 == 1) {
            cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N,
                        N, 1.0, R_prev, N, R, N, 0, R_cur, N);
            cblas_saxpy(N * N, 1.0, R_prev, 1, sum, 1);
        }
        else {
            cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N,
                        N, 1.0, R_cur, N, R, N, 0, R_prev, N);
            cblas_saxpy(N * N, 1.0, R_cur, 1, sum, 1);
        }
    }
    float* result = NewMatrix3(N);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0,
                sum, N, B, N, 0, result, N);
    DeleteMatrix3(I);
    DeleteMatrix3(sum);
    DeleteMatrix3(B);
    DeleteMatrix3(R);
    DeleteMatrix3(R_prev);
    DeleteMatrix3(R_cur);
    return result;
}

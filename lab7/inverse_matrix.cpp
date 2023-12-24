#include <cstdio>
#include <iostream>
#include "inverse_matrix.h"

void PrintMatrix1(const float* a, const int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%lf ", a[i * N + j]);
            std::cout << a[i * N + j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-----------------------------------------------------\n";
}

float* NewMatrix1(int N) {
    float* A = new float [N * N];
    return A;
}

void DeleteMatrix1(float* A) {
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

static void Add(const float* a, const float* b, float* res, const int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            res[i * N + j] = a[i * N + j] + b[i * N + j];
        }
    }
}

static void Subtract(const float* a, const float* b, float* res, const int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            res[i * N + j] = a[i * N + j] - b[i * N + j];
        }
    }
}

static void Divide(const float* a, const float divider, float* res, const int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            res[i * N + j] = a[i * N + j] / divider;
        }
    }
}

static float* Multiply(const float* a, const float* b, float* res, const int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                res[i * N + k] += a[i * N + j] * b[j * N + k];
            }
        }
    }
    return res;
}

float* NewI1(const int N) {
    float* I = NewMatrix1(N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            I[i * N + j] = (i == j) ? 1 : 0;
        }
    }
    return I;
}

float* InvertMatrix1(const float* A, const int N) {
    float *sum = NewMatrix1(N), *I = NewI1(N), *B = NewMatrix1(N), *AT = NewMatrix1(N);
    Divide(Transpose(A, AT, N), CalcA1(A, N) * CalcAInf(A, N), B, N);
    DeleteMatrix1(AT);
    float *BA = NewMatrix1(N), *R = NewMatrix1(N);
    Multiply(B, A, BA, N);
    Subtract(I, BA, R, N);
    DeleteMatrix1(BA);
    float* prevR = NewMatrix1(N);
    Multiply(R, R, prevR, N);
    Add(I, R, sum, N);
    DeleteMatrix1(I);
    float* curR = NewMatrix1(N);
    for (int i = 3; i < 10; i++) {
        Add(sum, curR, sum, N);
        Multiply(R,curR,curR,N);
    }
    float* res = NewMatrix1(N);
    Multiply(sum, B, res, N);
    DeleteMatrix1(sum);
    DeleteMatrix1(B);
    DeleteMatrix1(R);
    DeleteMatrix1(prevR);
    DeleteMatrix1(curR);
    return res;
}

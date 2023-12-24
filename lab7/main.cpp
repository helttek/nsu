#include"inverse_blas.h"
#include<iostream>
#include "inverse_matrix.h"
#include <random>
#include "inverse_fast.h"
#include <chrono>

float* InitializeMatrix(float* A, const int N) {
    srand(time(nullptr));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 10.0);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i * N + j] = dis(gen); //dis(gen)
        }
    }
    return A;
}

int main() {
    const int N = 512;
    float *I = NewI2(N), *A = NewMatrix2(N);
    InitializeMatrix(A, N);
    // PrintMatrix(A, N);
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    // PrintMatrix(AInv, N);
    float* AInv1 = InvertMatrix1(A, N);
    // PrintMatrix(AInv, N);
    std::chrono::high_resolution_clock::time_point finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dur = std::chrono::duration_cast<std::chrono::duration<double>>(
        finish - start);
    std::cout << "Slow: " << dur.count() << '\n';
    InitializeMatrix(A, N);
    start = std::chrono::high_resolution_clock::now();
    float* AInv2 = InvertMatrix2(A, N);
    finish = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration_cast<std::chrono::duration<double>>(
        finish - start);
    std::cout << "Fast: " << dur.count() << '\n';
    InitializeMatrix(A, N);
    start = std::chrono::high_resolution_clock::now();
    float* AInv3 = InvertMatrix3(A, N);
    finish = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration_cast<std::chrono::duration<double>>(
        finish - start);
    std::cout << "BLAS: " << dur.count() << '\n';
    DeleteMatrix3(AInv3);
    DeleteMatrix1(AInv1);
    DeleteMatrix2(AInv2);
    DeleteMatrix2(A);
    DeleteMatrix2(I);
    return EXIT_SUCCESS;
}

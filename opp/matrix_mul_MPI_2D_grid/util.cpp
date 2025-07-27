#include "util.h"
#include <iostream>

void InitMat(float *mat, const int n1, const int n2, const float value) {
  for (int i = 0; i < n1; i++) {
    for (int j = 0; j < n2; j++) {
      mat[i * n2 + j] = value;
    }
  }
}

void Mul(const float *A, const float *B, float *res, const int n1, const int n2,
         const int n3) {
  for (int i = 0; i < n1; ++i) {
    for (int j = 0; j < n3; ++j) {
      for (int k = 0; k < n2; ++k) {
        res[i * n3 + j] += A[i * n2 + k] * B[k * n3 + j];
      }
    }
  }
}

void PrintMatrix(const float *mat, const int n1, const int n2) {
  for (int i = 0; i < n1; ++i) {
    for (int j = 0; j < n2; ++j) {
      if (j == n2 - 1) {
        std::cout << mat[i * n2 + j] << '\n';
      } else {
        std::cout << mat[i * n2 + j] << ", ";
      }
    }
  }
}
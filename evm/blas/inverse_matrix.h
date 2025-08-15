#ifndef LAB7_INVERSE_MATRIX_H
#define LAB7_INVERSE_MATRIX_H

void PrintMatrix1(const float* a, const int N);

float* NewMatrix1(int N);

void DeleteMatrix1(float* a);

float* NewI1(int N);

// float* MultiplyMatrices(const float* a, const float* b, float* res, int N);

float* InvertMatrix1(const float* a, int N);

#endif //LAB7_INVERSE_MATRIX_H

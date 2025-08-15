#ifndef INVERSE_BLAS_H
#define INVERSE_BLAS_H
void PrintMatrix1(const float* a, const int N);

float* NewMatrix3(int N);

void DeleteMatrix3(float* a);

float* NewI3(int N);

float* InvertMatrix3(const float* a, int N);
#endif //INVERSE_BLAS_H

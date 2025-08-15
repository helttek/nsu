#ifndef INVERSE_FAST_H
#define INVERSE_FAST_H

void PrintMatrix2(const float* a, int N);

float* NewMatrix2(int N);

void DeleteMatrix2(float* a);

float* NewI2(int N);

float* InvertMatrix2(float* a, int N);

#endif //INVERSE_FAST_H

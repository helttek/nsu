#include <iostream>
#include <immintrin.h>

void PrintMatrix2(const float* a, const int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << a[i * N + j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-----------------------------------------------------\n";
}

float* NewMatrix2(int N) {
    auto* A = new float [N * N];
    return A;
}

void DeleteMatrix2(float* A) {
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
    for (int i = 0; i < N * N; i += 8) {
        __m256 r1 = _mm256_loadu_ps(a + i);
        __m256 r2 = _mm256_loadu_ps(b + i);
        __m256 sum = _mm256_add_ps(r1, r2);
        _mm256_storeu_ps(res + i, sum);
    }
}

static void Subtract(const float* a, const float* b, float* res, const int N) {
    for (int i = 0; i < N * N; i += 8) {
        __m256 r1 = _mm256_loadu_ps(a + i);
        __m256 r2 = _mm256_loadu_ps(b + i);
        __m256 sum = _mm256_sub_ps(r1, r2);
        _mm256_storeu_ps(res + i, sum);
    }
}

static void Divide(const float* a, const float divider, float* res, const int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            res[i * N + j] = a[i * N + j] / divider;
        }
    }
}

static float* Multiply(float* a, float* b, float* res, const int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j += 8) {
            __m256 m0 = _mm256_setzero_ps();
            for (int k = 0; k < N; k++) {
                __m256 m1 = _mm256_set1_ps(a[i * N + k]); //_mm256_broadcast_ss
                // std::cout << "index: " << k * N + j << std::endl;
                __m256 m2 = _mm256_loadu_ps((b + k * N + j));
                __m256 m3 = _mm256_mul_ps(m1, m2);
                m0 = _mm256_add_ps(m0, m3);
            }
            _mm256_storeu_ps(res + i * N + j, m0);
        }
    }
    return res;
}

float* NewI2(const int N) {
    float* I = NewMatrix2(N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            I[i * N + j] = (i == j) ? 1 : 0;
        }
    }
    return I;
}

float* InvertMatrix2(float* A, const int N) {
    float *sum = NewMatrix2(N), *I = NewI2(N), *B = NewMatrix2(N), *AT = NewMatrix2(N);
    Divide(Transpose(A, AT, N), CalcA1(A, N) * CalcAInf(A, N), B, N);
    DeleteMatrix2(AT);
    float *BA = NewMatrix2(N), *R = NewMatrix2(N);
    Multiply(B, A, BA, N);
    Subtract(I, BA, R, N);
    DeleteMatrix2(BA);
    float* curR = NewMatrix2(N);
    Multiply(R, R, curR, N);
    Add(I, R, sum, N);
    DeleteMatrix2(I);
    for (int i = 3; i < 10; i++) {
        Add(sum, curR, sum, N);
        Multiply(R,curR,curR,N);
    }
    float* res = NewMatrix2(N);
    Multiply(sum, B, res, N);
    DeleteMatrix2(sum);
    DeleteMatrix2(B);
    DeleteMatrix2(R);
    DeleteMatrix2(curR);
    return res;
}

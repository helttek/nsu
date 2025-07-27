#include <iostream>
#include <climits>
#include <x86intrin.h>

void InitializeArray(int* arr, int fragments, int offset) {
    int i, j;
    for (i = 0; i < offset; ++i) {
        for (j = 0; j < fragments - 1; j++) {
            arr[j * offset + i] = (j + 1) * offset + i;
        }
        arr[j * offset + i] = i + 1;
    }
    arr[j * offset + i] = 0;
}

void MeasureTime(int* arr, int offset, int fragments) {
    InitializeArray(arr, fragments, offset);
    int min = INT_MAX, n = fragments * offset;
    for (int i = 0; i < 500; i++) {
        int j = 0;
        int start = _rdtsc();
        for (int k = 0; k < n; ++k) {
            j = arr[j];
        }
        int finish = _rdtsc();
        min = (finish - start < min) ? (finish - start) : min;
    }
    std::cout << fragments << " fragments: " << min / n << " ticks\n";
}

int main() {
    auto* arr = new int [30000000];
    const int Offset = 1024; //4KB - one bank size (8 banks in total) -- 262144
    for (int fragment = 1; fragment <= 32; fragment++) {
        MeasureTime(arr, Offset, fragment);
    }
    operator delete [](arr);
    return EXIT_SUCCESS;
}

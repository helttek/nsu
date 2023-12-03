#include <vector>
#include <iostream>
#include <immintrin.h>
#include <climits>
#include <random>
#include <algorithm>
#include <fstream>
#include "../plots/pbPlots.hpp"
#include "../plots/supportLib.hpp"

void DirectTraversal(size_t *arr, size_t sz) {
    for (size_t i = 0; i < sz - 1; ++i) {
        arr[i] = i + 1;
    }
    arr[sz - 1] = 0;
}

size_t MinSize_t(const size_t a, const size_t b) {
    return (a < b) ? a : b;
}

#pragma optimize( "", off )

size_t Traverse(size_t *arr, const size_t size, const size_t k) {
    size_t l = 0, start = _rdtsc();
    for (size_t i = 0; i < size * k; ++i)
        l = static_cast<size_t volatile *>(arr)[l];
    size_t finish = _rdtsc();
//    std::cout << "finish = " << finish << ", start = " << start << std::endl;
//    std::cout << "finish - start = " << finish - start << std::endl;
    return finish - start;
}

#pragma optimize( "", on )

void ReverseTraversal(size_t *arr, size_t sz) {
    for (long long i = sz - 1; i > 0; --i) {
        arr[i] = i - 1;
    }
    arr[0] = sz - 1;
}

void RandomTraversal(size_t *arr, size_t sz) {
    for (size_t i = 0; i < sz; ++i) {
        arr[i] = i;
    }
    unsigned seed = 0;
    std::shuffle(arr, arr + sz, std::default_random_engine(seed));
}

int main() {
    srand(time(nullptr));
    const size_t MIN_SIZE = 128, MAX_SIZE = 1835008;//should be: l1 - 3072, l2 - 196608, l3 - 1572864 (for 8-byte elements)
    auto *arr = new size_t[MAX_SIZE];
    std::vector<double> x, y1, y2, y3;//coordinates of the plot
    const size_t repeat = 5;//how many times to traverse the array

    for (size_t i = MIN_SIZE; i < MAX_SIZE; i = (size_t) (i * 1.02)) {
        size_t min = ULLONG_MAX;
        std::cout << "array size: " << i << std::endl;

        x.push_back(i);
        DirectTraversal(arr, i);
        for (size_t j = 0; j < 3; j++) {
            min = MinSize_t(Traverse(arr, i, repeat), min);
        }
        y1.push_back((double) min / (double) (i * repeat));
        std::cout << "direct traverse: " << min / (i * repeat) << std::endl;

        min = ULLONG_MAX;
        ReverseTraversal(arr, i);
        for (size_t j = 0; j < 3; j++) {
            min = MinSize_t(Traverse(arr, i, repeat), min);
        }
        y2.push_back((double) min / (double) (i * repeat));
        std::cout << "reverse traverse: " << min / (i * repeat) << std::endl;

        min = ULLONG_MAX;
        RandomTraversal(arr, i);
        for (size_t j = 0; j < 3; j++) {
            min = MinSize_t(Traverse(arr, i, repeat), min);
        }
        y3.push_back((double) min / (double) (i * repeat));
        std::cout << "random traverse: " << min / (i * repeat) << std::endl;

        std::cout << "---------------------------------------------------------------------" << std::endl;
    }
    delete[] arr;

    std::ofstream out("coordinates.csv");
    if (!out.is_open())std::cout << "wtf\n";
    for (int i = 0; i < x.size(); ++i) {
        out << x[i] << ", " << y1[i] << ", " << y2[i] << ", " << y3[i] << std::endl;
    }

    return EXIT_SUCCESS;
}
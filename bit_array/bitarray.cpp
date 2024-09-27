// В этой задаче для простоты не требуется делать контейнер шаблонным,
// но это вполне допускается по желанию студента.

#include "bitarray.h"
#include <iostream>
#include <vector>
#include <string>
#include <climits>

int BitArray::CalcBytes(int n) {
    return (n % 8 == 0) ? (n / 8) : (n / 8 + 1);
}

int BitArray::Min(int a, int b) {
    return (a < b) ? (a) : (b);
}

int BitArray::CountOnes(unsigned char a, int end) {
    int res = 0;
    for (size_t i = 0; i < end; i++) {
        if ((a & 128) == 128) {
            ++res;
        }
        a <<= 1;
    }
    return res;
}

std::string BitArray::ByteToString(unsigned char a, const int n) {
    std::string res;
    for (size_t i = 0; i < n; i++) {
        if ((a & 128) == 128) {
            res += "1";
        }
        else {
            res += "0";
        }
        a <<= 1;
    }
    return res;
}

BitArray::BitArray() : arr(), numBits{0}, numBytes{0} {
}

BitArray::~BitArray() = default;

BitArray::BitArray(const int num_bits, const unsigned long value) : arr(CalcBytes(num_bits)), numBits{num_bits},
                                                                    numBytes{CalcBytes(num_bits)} {
    if (num_bits <= 0) {
        throw std::underflow_error("Error: number of bits to allocate is equal or less than zero.");
    }
    for (size_t i = 0; i < Min(sizeof(long), numBytes); ++i) {
        arr[i] = value >> ((sizeof(unsigned long) - i - 1) * 8);
    }
}

BitArray::BitArray(const BitArray&b) : arr(b.numBytes), numBits{b.numBits}, numBytes{b.numBytes} {
    for (int i = 0; i < numBytes; ++i) {
        arr[i] = b.arr[i];
    }
}

void BitArray::swap(BitArray&b) {
    arr.swap(b.arr);
    int tmp = numBits;
    numBits = b.numBits;
    b.numBits = tmp;
    tmp = numBytes;
    numBytes = b.numBytes;
    b.numBytes = tmp;
}

BitArray& BitArray::operator=(const BitArray&b) {
    if (b.numBits != numBits) {
        arr.resize(b.numBytes);
    }
    this->numBits = b.size();
    this->numBytes = b.numBytes;
    for (int i = 0; i < numBytes; ++i) {
        arr[i] = b.arr[i];
    }
    return *this;
}

void BitArray::resize(int num_bits, bool value) {
    if (num_bits < 0) {
        throw std::underflow_error("Error: New size of array is negative.");
    }
    arr.resize(CalcBytes(num_bits), false);
    if (num_bits > numBits) // new > current
    {
        int offset = 8 - (numBits % 8);
        if (numBytes == 0) // if arr is empty
        {
            numBytes = 1;
        }
        arr[numBytes - 1] >>= offset;
        for (int i = 0; i < offset; ++i) {
            arr[numBytes - 1] <<= 1;
            if (value == true) // else not needed because bit shift adds zeros
            {
                ++arr[numBytes - 1];
            }
        }
        for (int i = numBytes;
             i < CalcBytes(num_bits); i++) // else not needed because vector resize sets all new bits to zero
        {
            if (value == true) {
                arr[i] = 255; // set all bits in a byte to true
            }
        }
    }
    this->numBits = num_bits;
    this->numBytes = CalcBytes(num_bits);
}

void BitArray::clear() {
    arr.clear();
    numBits = 0;
    numBytes = 0;
}

void BitArray::push_back(bool bit) {
    if (numBits % 8 == 0) {
        this->numBytes++;
        arr.resize(numBytes);
    }
    int offset = 8 - (numBits % 8);
    arr[numBytes - 1] >>= offset;
    for (int i = 0; i < offset; ++i) // putting bits back in place
    {
        arr[numBytes - 1] <<= 1;
        if (bit == true) {
            arr[numBytes - 1]++;
        }
    }
    this->numBits++;
}

BitArray& BitArray::operator&=(const BitArray&b) {
    for (size_t i = 0; i < Min(numBytes, b.numBytes); i++) {
        arr[i] &= b.arr[i];
    }
    return *this;
}

BitArray& BitArray::operator|=(const BitArray&b) {
    for (size_t i = 0; i < Min(numBytes, b.numBytes); i++) {
        arr[i] |= b.arr[i];
    }
    return *this;
}

BitArray& BitArray::operator^=(const BitArray&b) {
    for (size_t i = 0; i < Min(numBytes, b.numBytes); i++) {
        arr[i] ^= b.arr[i];
    }
    return *this;
}

BitArray& BitArray::operator<<=(const int n) {
    if (n <= 0) {
        throw std::underflow_error("Error: Negative bit shift is undefined.");
    }
    int chars = n / 8; // number of byte shifts
    if (n >= numBits) {
        for (int i = 0; i < numBytes; ++i) {
            arr[i] = 0;
        }
        return *this;
    }
    if (chars > 0) {
        int size = CalcBytes(numBits);
        for (int i = 0; (i + chars) < size; i++) {
            arr[i] = arr[i + chars];
        }
        for (int i = size; chars > 0; chars--) {
            //nullify bits on the right
            arr[i - chars] = 0;
        }
    }
    for (int i = 0; i < (n % 8); i++) {
        for (unsigned int j = 0; j < (numBits - 1) / 8; j++) {
            arr[j] <<= 1; // dealing with borders of a byte
            if (arr[j + 1] & 128) {
                arr[j] |= 1;
            }
        }
        arr[(numBits - 1) / 8] <<= 1;
    }
    return *this;
}

BitArray& BitArray::operator>>=(int n) {
    if (n > numBits) {
        for (int i = 0; i < numBytes; ++i) {
            arr[i] = 0;
        }
        return *this;
    }
    if (n <= 0) {
        throw std::underflow_error("Error: Negative bit shift is undefined.");
    }
    int chars = n / 8; // number of byte shifts
    char mask;
    for (int i = numBytes; (i - chars) >= 0; i--) // shifting bytes to the right end of the array
    {
        arr[i] = arr[i - chars];
    }
    for (/**/; chars > 0; chars--) {
        arr[chars - 1] = 0;
    }
    for (int i = 0; i < (n % 8); i++) {
        for (int j = numBytes; j > 0; j--) // спихиваем все биты на один в вправо
        {
            arr[j] >>= 1;
            if (arr[j - 1] & 1) // dealing with borders of a byte
            {
                arr[j] |= 128;
            }
        }
        arr[0] >>= 1;
    }
    int i = numBits % 8;
    if (i != 0) // nullifying the rest of the array
    {
        mask = UCHAR_MAX << (8 - i);
        arr[(numBits - 1) / 8] &= mask;
    }
    return *this;
}

BitArray BitArray::operator<<(int n) const {
    BitArray res(*this);
    res <<= n;
    return res;
}

BitArray BitArray::operator>>(int n) const {
    BitArray res(*this);
    res >>= n;
    return res;
}

BitArray& BitArray::set(int n, bool val) {
    if (n < 0) {
        throw std::underflow_error("Error: Negative array index.");
    }
    if (n > numBits) {
        throw std::overflow_error("Error: Index is greater than the number of elements in array.");
    }
    int indexByte = CalcBytes(n + 1) - 1, right = 8 - ((n + 1) % 8), left = ((n + 1) % 8);
    if ((n + 1) % 8 == 0) {
        if ((arr[indexByte] % 2) == 0) {
            arr[indexByte] |= (unsigned char)val;
        }
        else {
            arr[indexByte] &= (254 + (unsigned char)val);
        }
        return *this;
    }
    unsigned char tmp = arr[indexByte] << left;
    tmp >>= left;
    arr[indexByte] >>= right;
    if (val) {
        arr[indexByte] |= 1;
    }
    else {
        arr[indexByte] >>= 1;
        arr[indexByte] <<= 1;
    }
    arr[indexByte] <<= right;
    arr[indexByte] |= tmp;
    return *this;
}

BitArray& BitArray::set() {
    for (size_t i = 0; i < numBytes; i++) {
        arr[i] = 255;
    }
    return *this;
}

// Устанавливает бит с индексом n в значение false.
BitArray& BitArray::reset(const int n) {
    this->set(n, false);
    return *this;
}

BitArray& BitArray::reset() {
    for (size_t i = 0; i < numBytes; i++) {
        arr[i] = 0;
    }
    return *this;
}

bool BitArray::any() const {
    for (size_t i = 0; i < numBytes; i++) {
        if (arr[i] != 0) {
            return true;
        }
    }
    return false;
}

bool BitArray::none() const {
    for (size_t i = 0; i < numBytes; i++) {
        if (arr[i] != 0) {
            return false;
        }
    }
    return true;
}

BitArray BitArray::operator~() const {
    BitArray res(*this);
    for (size_t i = 0; i < numBytes; i++) {
        res.arr[i] = ~arr[i];
    }
    return res;
}

int BitArray::count() const {
    int res = 0;
    for (size_t i = 0; i < numBytes - 1; i++) {
        res += CountOnes(arr[i], 8);
    }
    res += (numBits % 8 == 0) ? (CountOnes(arr[numBytes - 1], 8)) : (CountOnes(arr[numBytes - 1], numBits % 8));
    return res;
}

bool BitArray::operator[](int i) const {
    if (i < 0) {
        throw std::underflow_error("Error: Negative array index.");
    }
    if (i > numBits - 1) {
        throw std::overflow_error("Error: Index is greater than the number of elements in the array.");
    }
    int indexByte = CalcBytes(i + 1) - 1, right = 8 - ((i + 1) % 8);
    if ((i + 1) % 8 == 0) {
        if ((arr[indexByte] & 1) == 1) {
            return true;
        }
        return false;
    }
    unsigned char tmp = arr[indexByte] >> right;
    if ((tmp & 1) == 1) {
        return true;
    }
    return false;
}

int BitArray::size() const {
    return numBits;
}

bool BitArray::empty() const {
    return (numBits <= 0);
}

// Возвращает строковое представление массива.
std::string BitArray::to_string() const {
    if (arr.empty()) return "";
    std::string res;
    for (size_t i = 0; i < numBytes - 1; i++) {
        res += ByteToString(arr[i], 8);
    }
    res += (numBits % 8 == 0) ? (ByteToString(arr[numBytes - 1], 8)) : (ByteToString(arr[numBytes - 1], numBits % 8));
    return res;
}

bool operator==(const BitArray&a, const BitArray&b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const BitArray&a, const BitArray&b) {
    if (a.size() != b.size()) {
        return true;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            return true;
        }
    }
    return false;
}

BitArray operator&(const BitArray&b1, const BitArray&b2) {
    BitArray a(b1), b(b2);
    a &= b;
    return a;
}

BitArray operator|(const BitArray&b1, const BitArray&b2) {
    BitArray a(b1), b(b2);
    a |= b;
    return a;
}

BitArray operator^(const BitArray&b1, const BitArray&b2) {
    BitArray a(b1), b(b2);
    a ^= b;
    return a;
}

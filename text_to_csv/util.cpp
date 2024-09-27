#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "util.h"

static bool cmp(std::pair<std::wstring, int> &a, std::pair<std::wstring, int> &b) {
    return a.second > b.second;
}

int CheckFiles(int N, const std::string &input, const std::string &output) {
    if (N != 3) {
        std::cout << "Wrong number of files passed to an executable\n";
        return EXIT_FAILURE;
    }
    size_t len = input.length();
    if (input[len - 4] != '.' || input[len - 3] != 't' || input[len - 2] != 'x' || input[len - 1] != 't') {
        std::cout << "wrong input file format\n";
        return EXIT_FAILURE;
    }
    len = output.length();
    if (output[len - 4] != '.' || output[len - 3] != 'c' || output[len - 2] != 's' || output[len - 1] != 'v') {
        std::cout << "wrong output file format\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void DissectLine(std::wstring &line, std::map<std::wstring, int> &words, int &wordCount) {
    int first = NOT_FIRST, last = NOT_LAST;
    std::wstring tmp;
    for (size_t i = 0; i < line.length(); i++) {
        if (!isalnum(line[i])) {
            if (last != NOT_LAST) {
                tmp = line.substr(first, last - first + 1);
                if (words.find(tmp) != words.end())
                    words.insert({tmp, 0});
                words[tmp]++;
                wordCount++;
                last = NOT_LAST;
                first = NOT_FIRST;
            }
        } else {
            if (i == line.length() - 1) {//if last character is not a delimiter
                if (first == NOT_FIRST)
                    first = line.length() - 1;
                tmp = line.substr(first, line.length() - first);
                if (words.find(tmp) != words.end())
                    words.insert({tmp, 0});
                words[tmp]++;
                wordCount++;
                last = NOT_LAST;
                first = NOT_FIRST;
            }
            if (first == NOT_FIRST)
                first = i;
            last = i;
        }
    }
}

int ReadInput(std::wstring &line, std::map<std::wstring, int> &words, const std::string &fileName, int &wordCount) {
    std::wifstream in(fileName);
    if (in.fail()) {
        std::cerr << "Couldn't open input file\n";
        return EXIT_FAILURE;
    }
    while (std::getline(in, line)) {
        DissectLine(line, words, wordCount);
    }
    return EXIT_SUCCESS;
}

std::vector<std::pair<std::wstring, int>> SortWords(std::map<std::wstring, int> &map) {
    std::vector<std::pair<std::wstring, int>> vector;
    for (auto &i: map) {
        vector.emplace_back(i);
    }
    std::sort(vector.begin(), vector.end(), cmp);
    return vector;
}

void PrintVector(const std::vector<std::pair<std::wstring, int>>& vec, const std::string& fileName, int count) {
    std::wofstream out(fileName);
    for (auto & i : vec) {
        double freq = i.second / (double) count * 100;
        out << i.first << ", " << i.second << ", " << freq << "%" << std::endl;
    }
}
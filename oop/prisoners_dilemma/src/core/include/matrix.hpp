#pragma once

#include <vector>
#include <filesystem>

class Matrix
{
public:
    Matrix() = default;
    ~Matrix() = default;

    static std::vector<int> LoadRulesFromFile(std::filesystem::path matrixFile);
    static std::vector<int> LoadDefaultRules();

private:
};

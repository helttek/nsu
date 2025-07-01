#include "matrix.hpp"

#include <fstream>

std::vector<int> Matrix::LoadRulesFromFile(const std::filesystem::path matrixFile)
{
    std::vector<int> res(8 * 3);
    try
    {
        std::ifstream matrixStream(matrixFile);
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                matrixStream >> res[i * 3 + j];
            }
        }

        if (matrixStream.eof())
        {
            throw std::runtime_error("failed to read matrix from file: incorrect matrix format in file");
        }

        return std::vector<int>(res);
    }
    catch (std::exception e)
    {
        throw std::runtime_error(std::string("failed to read from matrix file: ") + e.what());
    }
}

std::vector<int> Matrix::LoadDefaultRules()
{
    return std::vector<int>({7, 7, 7, 3, 3, 9, 3, 9, 3, 9, 3, 3, 0, 5, 5, 5, 0, 5, 5, 5, 0, 1, 1, 1});
}
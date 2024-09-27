#include "NewtonPolynomial.hpp"
#include <fstream>

NewtonPolynomial::NewtonPolynomial(const std::vector<double> nodes) : nodes(nodes)
{
    for (size_t i = 0; i < nodes.size(); i++)
    {
        /* code */
    }
}

NewtonPolynomial::~NewtonPolynomial()
{
}

void NewtonPolynomial::CalcAndDump(std::string outFile)
{
    std::ofstream out(outFile);
}
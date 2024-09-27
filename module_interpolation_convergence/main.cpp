#include <iostream>
#include <fstream>
#include "NewtonPolynomial.hpp"

int main()
{
    std::vector<double> nodes(11);
    NewtonPolynomial newPol(nodes);
    newPol.CalcAndDump("output.txt");
    return EXIT_SUCCESS;
}
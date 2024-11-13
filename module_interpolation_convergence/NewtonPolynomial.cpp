#include "NewtonPolynomial.hpp"
#include <fstream>
#include <iostream>

NewtonPolynomial::NewtonPolynomial(const std::vector<double> nodes)
    : nodes(nodes) {}

NewtonPolynomial::~NewtonPolynomial() {}

void NewtonPolynomial::CalcAndDump(std::string outFile) {
  std::ofstream out(outFile);
  if (!out.is_open()) {
    std::cerr << "Error: couldn't open output file\n";
    return;
  }
  for (size_t i = 0; i < nodes.size(); ++i) {
    for (size_t j = 0; j < i; ++j) {
    }
  }
}

double NewtonPolynomial::DivDif() { return 1; }

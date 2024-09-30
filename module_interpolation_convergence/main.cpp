#include "NewtonPolynomial.hpp"
#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

void ReadInput(std::string in, std::vector<double> &nodes) {
  std::ifstream fin(in);
  size_t i = 0;
  while (i < nodes.size() && !fin.eof()) {
    fin >> nodes[i];
  }
}

int main() {
  std::vector<double> nodes(11);
  ReadInput("input", nodes);
  NewtonPolynomial newPol(nodes);
  newPol.CalcAndDump("output.txt");
  return EXIT_SUCCESS;
}

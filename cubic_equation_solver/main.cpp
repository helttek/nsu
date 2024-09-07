#include "CubicEquationSolver.hpp"
#include <fstream>
#include <iostream>
#include <vector>

void GetInput(double &a, double &b, double &c, double &d, double &epsilon) {
  std::cout << "Введите эпсилон: ";
  std::cin >> epsilon;
  std::cout << "Введите параметр a: ";
  std::cin >> a;
  std::cout << "Введите параметр b: ";
  std::cin >> b;
  std::cout << "Введите параметр c: ";
  std::cin >> c;
  std::cout << "Введите параметр d: ";
  std::cin >> d;
}

void GetFileInput(std::string filename, double &a, double &b, double &c,
                  double &d, double &epsilon) {
  std::ifstream fin(filename);
  if (!fin.is_open()) {
    throw std::runtime_error("Failed to open input file.");
  }
  fin >> epsilon >> a >> b >> c >> d;
}

int main() {
  double a, b, c, d, epsilon;
  GetFileInput("input", a, b, c, d, epsilon);
  CubicEquationSolver cubicEquationSolver(a, b, c, d, epsilon);
  std::vector<double> res, quadRes;
  cubicEquationSolver.Solve(res);
  // QuadraticEquation quadEq(1, 4, -5);
  // quadEq.Solve(quadRes);
  // for (int i = 0; i < quadRes.size(); ++i) {
  //   std::cout << "root " << i + 1 << ": " << quadRes[i] << '\n';
  // }
  return 0;
}

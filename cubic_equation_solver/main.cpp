#include "CubicEquationSolver.hpp"
#include <fstream>
#include <iomanip>
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

/*
 * 1 -2 -1 2 (-1 1 2)
 * 2 -2 -1 2 (-0.8755)
 * 1 1 1 -3 (1)
 * 6 0 0 1 (-0.55)
 * 5 3 -1.8 0.2 (-1 0.2)
 * 1 0 0 0 (0 0 0)
 */

int main() {
  double a, b, c, d, epsilon;
  GetFileInput("input", a, b, c, d, epsilon);
  // GetInput(a, b, c, d, epsilon);
  CubicEquationSolver cubicEquationSolver(a, b, c, d, epsilon);
  std::vector<double> res, quadRes;
  cubicEquationSolver.Solve(res);
  for (size_t i = 0; i < res.size(); ++i) {
    std::cout << std::setprecision(15) << "x" << i + 1 << ": " << res[i]
              << '\n';
  }
  return 0;
}

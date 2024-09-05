#include "CubicEquationSolver.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

CubicEquationSolver::CubicEquationSolver() {
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

CubicEquationSolver::CubicEquationSolver(std::string filename) {
  std::ifstream fin(filename);
  if (!fin.is_open()) {
    throw std::runtime_error("Failed to open input file.");
  }
  fin >> epsilon >> a >> b >> c >> d;
  // std::cout << epsilon << "\n" << a << '\n' << b << '\n' << c << '\n' << d;
}

void CubicEquationSolver::FindDerivativeCoefficents(double a, double b,
                                                    double c,
                                                    std::vector<double> &res) {
  res.resize(3);
  res[0] = a * 3;
  res[1] = b * 2;
  res[2] = c;
}

double CubicEquationSolver::f(double x) {
  return a * x * x * x + b * x * x + c * x + d;
}

double CubicEquationSolver::GetSolutionFromSegment() { return true; }

bool CubicEquationSolver::FirstOption(std::vector<double> &res) {
  if (f(0) == 0) {
    res.resize(1);
    res[0] = 0;
    return true;
  }
  if (f(0) > 0) {
    res.resize(1);
    res[0] = GetSolutionFromSegment();
    return true;
  }
  res.resize(1);
  res[0] = GetSolutionFromSegment();
  return true;
}

void CubicEquationSolver::SecondOption(std::vector<double> solutions,
                                       std::vector<double> &res) {
  double x1 = std::min(solutions[0], solutions[1]),
         x2 = std::max(solutions[0], solutions[1]);
  if (f(x2) > 0) { // II 1
  }
  if (f(x1) < 0) { // II 2
  }
  if (f(x1) > epsilon && f(x2) < -epsilon) { // II 3
  }
  if (std::abs(f(x2)) < epsilon && std::abs(f(x1)) > epsilon) { // III 1
  }
  if (std::abs(f(x1)) < epsilon && std::abs(f(x2)) < epsilon) { // III 2
  }
}

void CubicEquationSolver::ThirdOption(double derivSolution,
                                      std::vector<double> &res) {
  if (f(derivSolution) > epsilon) { // IV 1
  }
  if (f(derivSolution) < -epsilon) { // IV 2
  }
}

void CubicEquationSolver::Solve(std::vector<double> &res) {
  std::vector<double> derivCoef, quadSolutions;
  FindDerivativeCoefficents(a, b, c, derivCoef);
  QuadraticEquation quadEq(derivCoef[0], derivCoef[1], derivCoef[2]);
  double d = quadEq.GetDiscriminant();
  quadEq.Solve(quadSolutions);
  if (d < 0) { // TODO: check logic
    FirstOption(res);
  } else {
    if (d > 0) {
      SecondOption(quadSolutions, res);
      return;
    }
    ThirdOption(quadSolutions[0], derivCoef);
  }
}

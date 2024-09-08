#include "CubicEquationSolver.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>

CubicEquationSolver::CubicEquationSolver(double a, double b, double c, double d,
                                         double epsilon)
    : a(a), b(b), c(c), d(d), epsilon(epsilon), segSol(a, b, c, d, epsilon) {}

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

void CubicEquationSolver::FirstOption(std::vector<double> &res) {
  if (f(0) == 0) { // 1.1
    res.resize(1);
    res[0] = 0;
    std::cout << "option: 1.1\n";
    return;
  }
  if (a > 0) {
    if (f(0) < 0) { // 1.2
      res.resize(1);
      res[0] = segSol.AToInf(0);
      std::cout << "option: 1.2\n";
      return;
    }
    res.resize(1);
    res[0] = segSol.InfToA(0); // 1.3
    std::cout << "option: 1.3\n";
  } else {
    if (f(0) < 0) { // 1.4
      res.resize(1);
      res[0] = segSol.InfToA(0);
      std::cout << "option: 1.4\n";
      return;
    }
    res.resize(1);
    res[0] = segSol.AToInf(0); // 1.5
    std::cout << "option: 1.5\n";
  }
}

void CubicEquationSolver::SecondOption(std::vector<double> solutions,
                                       std::vector<double> &res) {
  double lower =
      (f(solutions[0]) < f(solutions[1])) ? solutions[0] : solutions[1];
  double higher =
      (f(solutions[0]) > f(solutions[1])) ? solutions[0] : solutions[1];
  if (a > 0) {
    if (f(lower) > 0) { // 2.1
      res.resize(1);
      res[0] = segSol.InfToA(higher);
      std::cout << "option: 2.1\n";
      return;
    }
    if (f(higher) < 0) { // 2.2
      res.resize(1);
      res[0] = segSol.AToInf(lower);
      std::cout << "option: 2.2\n";
      return;
    }
    if (std::abs(f(lower)) < epsilon && std::abs(f(higher)) > epsilon) {
      res.resize(3); // 2.3
      res[0] = segSol.InfToA(lower);
      res[1] = lower;
      res[2] = lower;
      std::cout << "option: 2.3\n";
      return;
    }
    if (std::abs(f(higher)) < epsilon && std::abs(f(lower)) > epsilon) {
      res.resize(3); // 2.4
      res[0] = higher;
      res[1] = higher;
      res[2] = segSol.AToInf(higher);
      std::cout << "option: 2.4\n";
      return;
    }
    if (f(lower) < (-1) * epsilon && f(higher) > epsilon) {
      res.resize(3); // 2.5
      res[0] = segSol.InfToA(higher);
      res[1] = segSol.AToB(higher, lower);
      res[2] = segSol.AToInf(lower);
      std::cout << "option: 2.5\n";
      return;
    }
  }
  if (f(higher) < 0) { // 2.6
    res.resize(1);
    res[0] = segSol.InfToA(lower);
    std::cout << "option: 2.6\n";
  }
  if (f(lower) > 0) { // 2.7
    res.resize(1);
    res[0] = segSol.AToInf(higher);
    std::cout << "option: 2.7\n";
  }
  if (std::abs(f(lower)) < epsilon && std::abs(f(higher)) > epsilon) {
    res.resize(3); // 2.8
    res[0] = segSol.AToInf(higher);
    res[1] = lower;
    res[2] = lower;
    std::cout << "option: 2.8\n";
  }
  if (std::abs(f(higher)) < epsilon && std::abs(f(lower)) > epsilon) {
    res.resize(3); // 2.9
    res[0] = higher;
    res[1] = higher;
    res[2] = segSol.InfToA(lower);
    std::cout << "option: 2.9\n";
  }
  if (f(lower) < (-1) * epsilon && f(higher) > epsilon) {
    res.resize(3); // 2.10
    res[0] = segSol.InfToA(lower);
    res[1] = segSol.AToB(lower, higher);
    res[2] = segSol.AToInf(higher);
    std::cout << "option: 2.10\n";
  }
}

void CubicEquationSolver::ThirdOption(double derivSolution,
                                      std::vector<double> &res) {
  if (a > 0) {
    if (f(derivSolution) > epsilon) { // 3.1
      res.resize(1);
      res[0] = segSol.InfToA(derivSolution);
      std::cout << "option: 3.1\n";
      return;
    }
    if (f(derivSolution) < -epsilon) { // 3.2
      res.resize(1);
      res[0] = segSol.AToInf(derivSolution);
      std::cout << "option: 3.2\n";
      return;
    }
    res.resize(3);
    res[0] = (res[1] = (res[2] = derivSolution)); // 3.3
    std::cout << "option: 3.3\n";
    return;
  }
  if (f(derivSolution) > epsilon) { // 3.4
    res.resize(1);
    res[0] = segSol.AToInf(derivSolution);
    std::cout << "option: 3.4\n";
    return;
  }
  if (f(derivSolution) < -epsilon) { // 3.5
    res.resize(1);
    res[0] = segSol.InfToA(derivSolution);
    std::cout << "option: 3.5\n";
    return;
  }
  res.resize(3);
  res[0] = (res[1] = (res[2] = derivSolution)); // 3.6
  std::cout << "option: 3.6\n";
}

void CubicEquationSolver::Solve(std::vector<double> &res) {
  std::vector<double> derivCoef, quadSolutions;
  FindDerivativeCoefficents(a, b, c, derivCoef);
  QuadraticEquation quadEq(derivCoef[0], derivCoef[1], derivCoef[2]);
  double d = quadEq.GetDiscriminant();
  // std::cerr << "discriminant: " << d << '\n';
  quadEq.Solve(quadSolutions);
  // for (int i = 0; i < quadSolutions.size(); ++i) {
  //   std::cout << "root " << i + 1 << ": " << quadSolutions[i] << '\n';
  // }
  if (d < 0) {
    FirstOption(res);
  } else {
    if (d > 0) {
      SecondOption(quadSolutions, res);
      return;
    }
    ThirdOption(quadSolutions[0], res);
  }
}

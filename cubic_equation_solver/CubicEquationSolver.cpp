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
  if (f(0) == 0) {
    res.resize(1);
    res[0] = 0;
    return;
  }
  if (a > 0) {
    if (f(0) < 0) {
      res.resize(1);
      segSol.AToInf(0);
      return;
    }
    res.resize(1);
    segSol.InfToA(0);
  } else {
    if (f(0) < 0) {
      res.resize(1);
      segSol.InfToA(0);
      return;
    }
    res.resize(1);
    segSol.AToInf(0);
  }
}

void CubicEquationSolver::SecondOption(std::vector<double> solutions,
                                       std::vector<double> &res) {
  double lower =
      (f(solutions[0]) < f(solutions[1])) ? solutions[0] : solutions[1];
  double higher =
      (f(solutions[0]) > f(solutions[1])) ? solutions[0] : solutions[1];
  if (a > 0) {
    if (f(lower) > 0) {
      res.resize(1);
      res[0] = segSol.InfToA(higher);
      return;
    }
    if (f(higher) < 0) {
      res.resize(1);
      res[0] = segSol.AToInf(lower);
      return;
    }
    if (std::abs(f(lower)) < epsilon && std::abs(f(higher)) > epsilon) {
      res.resize(3);
      res[0] = segSol.InfToA(lower);
      res[1] = lower;
      res[2] = lower;
      return;
    }
    if (std::abs(f(higher)) < epsilon && std::abs(f(lower)) > epsilon) {
      res.resize(3);
      res[0] = higher;
      res[1] = higher;
      res[2] = segSol.AToInf(higher);
      return;
    }
    if (f(lower) < (-1) * epsilon && f(higher) > epsilon) {
      res.resize(3);
      res[0] = segSol.InfToA(higher);
      res[1] = segSol.AToB(higher, lower);
      res[2] = segSol.AToInf(lower);
    }
  }
  if (f(higher) < 0) {
    res.resize(1);
    res[0] = segSol.InfToA(lower);
  }
  if (f(lower) > 0) {
    res.resize(1);
    res[0] = segSol.AToInf(higher);
  }
  if (std::abs(f(lower)) < epsilon && std::abs(f(higher)) > epsilon) {
    res.resize(3);
    res[0] = segSol.AToInf(higher);
    res[1] = lower;
    res[2] = lower;
  }
  if (std::abs(f(higher)) < epsilon && std::abs(f(lower)) > epsilon) {
    res.resize(3);
    res[0] = higher;
    res[1] = higher;
    res[2] = segSol.InfToA(lower);
  }
  if (f(lower) < (-1) * epsilon && f(higher) > epsilon) {
    res.resize(3);
    res[0] = segSol.InfToA(lower);
    res[1] = segSol.AToB(lower, higher);
    res[2] = segSol.AToInf(higher);
  }
}

void CubicEquationSolver::ThirdOption(double derivSolution,
                                      std::vector<double> &res) {
  if (a > 0) {
    if (f(derivSolution) > epsilon) {
      res.resize(1);
      res[0] = segSol.InfToA(derivSolution);
      return;
    }
    if (f(derivSolution) < -epsilon) {
      res.resize(1);
      res[0] = segSol.AToInf(derivSolution);
      return;
    }
    res.resize(3);
    res[0] = (res[1] = (res[2] = derivSolution));
    std::cout << res[0] << '\n' << res[1] << '\n' << res[2] << '\n';
  }
  if (f(derivSolution) > epsilon) {
    res.resize(1);
    segSol.AToInf(derivSolution);
  }
  if (f(derivSolution) < -epsilon) {
    res.resize(1);
    segSol.InfToA(derivSolution);
  }
  res.resize(3);
  res[0] = (res[1] = (res[2] = derivSolution));
}

void CubicEquationSolver::Solve(std::vector<double> &res) {
  std::vector<double> derivCoef, quadSolutions;
  FindDerivativeCoefficents(a, b, c, derivCoef);
  QuadraticEquation quadEq(derivCoef[0], derivCoef[1], derivCoef[2]);
  double d = quadEq.GetDiscriminant();
  quadEq.Solve(quadSolutions);
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

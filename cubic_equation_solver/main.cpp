#include "CubicEquationSolver.hpp"
#include <iostream>
#include <vector>
int main() {
  CubicEquationSolver cubicEquationSolver("input");
  std::vector<double> res, quadRes;
  cubicEquationSolver.Solve(res);
  QuadraticEquation quadEq(1, 4, -5);
  quadEq.Solve(quadRes);
  // for (int i = 0; i < quadRes.size(); ++i) {
  //   std::cout << "root " << i + 1 << ": " << quadRes[i] << '\n';
  // }
  return 0;
}

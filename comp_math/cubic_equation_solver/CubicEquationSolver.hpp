#include "QuadraticEquation.hpp"
#include "SegmentSolution.hpp"
#include <cmath>
#include <vector>

class CubicEquationSolver {
public:
  CubicEquationSolver(double a, double b, double c, double d, double epsilon);
  void Solve(std::vector<double> &res);

private:
  double a, b, c, d, epsilon;
  SegmentSolution segSol;

  void FindDerivativeCoefficents(double a, double b, double c,
                                 std::vector<double> &res);

  double f(double x);

  void FirstOption(std::vector<double> &res);

  void SecondOption(std::vector<double> solutions, std::vector<double> &res);

  void ThirdOption(double derivSolution, std::vector<double> &res);
};

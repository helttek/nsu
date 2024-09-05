#include "QuadraticEquation.hpp"
#include <cmath>
#include <string>
#include <vector>

class CubicEquationSolver {
public:
  CubicEquationSolver();
  CubicEquationSolver(std::string filename);
  void Solve(std::vector<double> &res);

private:
  void FindDerivativeCoefficents(double a, double b, double c,
                                 std::vector<double> &res);

  bool FirstOption(std::vector<double> &res);

  double f(double x);

  double GetSolutionFromSegment();

  void SecondOption(std::vector<double> solutions, std::vector<double> &res);

  void ThirdOption(double derivSolution, std::vector<double> &res);

  double epsilon;
  double a, b, c, d;
};

#include <vector>
class QuadraticEquation {
public:
  QuadraticEquation(double a, double b, double c);
  void Solve(std::vector<double> &res);
  double GetDiscriminant();

private:
  double a, b, c;
};

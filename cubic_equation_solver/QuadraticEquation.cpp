#include "QuadraticEquation.hpp"
#include <cmath>
#include <vector>

QuadraticEquation::QuadraticEquation(double a, double b, double c)
    : a(a), b(b), c(c) {}

double QuadraticEquation::GetDiscriminant() { return b * b - 4 * a * c; }

void QuadraticEquation::Solve(std::vector<double> &res) {
  double d = GetDiscriminant();
  if (d < 0) {
    res.clear();
    return;
  }
  if (d == 0) {
    res.resize(1);
    res[0] = (-b) / (2 * a);
    return;
  }
  res.resize(2);
  res[0] = (-b + std::sqrt(d)) / (2 * a);
  res[1] = (-b - std::sqrt(d)) / (2 * a);
}

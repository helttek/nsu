#include "SegmentSolution.hpp"
#include <cstdlib>

SegmentSolution::SegmentSolution(const double a, const double b, const double c,
                                 const double d, const double epsilon)
    : a(a), b(b), c(c), d(d), epsilon(epsilon) {}

double SegmentSolution::f(const double x) {
  return a * x * x * x + b * x * x + c * x + d;
}

double SegmentSolution::InfToA(const double a) { return a; }

double SegmentSolution::AToInf(const double a) { return a; }

double SegmentSolution::AToB(const double a, const double b) {
  double z = (a + b) / 2, xn = a, yn = b;
  while (std::abs(xn - yn) >= epsilon) {
    // TODO: finish and test
    z = (xn + yn) / 2;
  }
  return z;
}

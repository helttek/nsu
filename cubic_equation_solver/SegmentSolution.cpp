#include "SegmentSolution.hpp"
#include <cstdlib>

SegmentSolution::SegmentSolution(const double a, const double b, const double c,
                                 const double d, const double epsilon)
    : a(a), b(b), c(c), d(d), epsilon(epsilon) {}

double SegmentSolution::f(const double x) {
  return a * x * x * x + b * x * x + c * x + d;
}

double SegmentSolution::InfToA(const double a) {
  double i = -1;
  while ((f(a) >= 0 && f(a + i * delta) >= 0) ||
         (f(a) <= 0 && f(a + i * delta) <= 0)) {
    --i;
    if (!f(a)) {
      return a;
    }
  }
  return AToB(a + i * delta, a);
}

double SegmentSolution::AToInf(const double a) {
  double i = 1;
  while ((f(a) >= 0 && f(a + i * delta) >= 0) ||
         (f(a) <= 0 && f(a + i * delta) <= 0)) {
    ++i;
    if (!f(a)) {
      return a;
    }
  }
  return AToB(a, a + i * delta);
}

double SegmentSolution::AToB(const double a, const double b) {
  double z = (a + b) / 2, xn = a, yn = b;
  while (std::abs(xn - yn) >= epsilon) {
    if ((f(xn) >= 0 && f(z) <= 0) || (f(xn) <= 0 && f(z) >= 0)) {
      yn = z;
      z = (xn + yn) / 2;
      continue;
    }
    if ((f(yn) >= 0 && f(z) <= 0) || (f(yn) <= 0 && f(z) >= 0)) {
      xn = z;
      z = (xn + yn) / 2;
      continue;
    }
  }
  return xn;
}

class SegmentSolution {
private:
  double f(const double x);
  double a, b, c, d, epsilon;

public:
  SegmentSolution(const double a, const double b, const double c,
                  const double d, const double epsilon);
  double InfToA(const double a);
  double AToInf(const double a);
  double AToB(const double a, const double b);
};
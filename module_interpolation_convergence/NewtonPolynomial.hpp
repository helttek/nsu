#include <string>
#include <vector>

class NewtonPolynomial {
private:
  std::vector<double> coef;
  std::vector<double> nodes;

public:
  NewtonPolynomial(const std::vector<double> nodes);
  ~NewtonPolynomial();
  void CalcAndDump(std::string outFile);
  // Divided difference
  double DivDif();
};

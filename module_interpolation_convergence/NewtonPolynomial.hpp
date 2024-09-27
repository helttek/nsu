#include <vector>
#include <string>

class NewtonPolynomial
{
private:
    std::vector<double> coef;
    std::vector<double> nodes;

public:
    NewtonPolynomial(const std::vector<double> nodes);
    ~NewtonPolynomial();
    void CalcAndDump(std::string outFile);
};

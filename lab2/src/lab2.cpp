#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <unistd.h>

#define GNU_SOURCE
#define INLINE
/*#define DEBUG*/

constexpr int Nx = 8000;
constexpr int Ny = 8000;
constexpr int N = Nx; // N==Nx==Ny
constexpr int Nt = 100;

constexpr double Xa = 0.0;
constexpr double Xb = 4.0;
constexpr double Ya = 0.0;
constexpr double Yb = 4.0;

constexpr double hx = (Xb - Xa) / (Nx - 1);
constexpr double hy = (Yb - Ya) / (Ny - 1);

constexpr double YS1 = Ya + (Yb - Ya) * 2 / 3;
constexpr double YS2 = Ya + (Yb - Ya) / 3;
constexpr double XS1 = Xa + (Xb - Xa) / 3;
constexpr double XS2 = Xa + (Xb - Xa) * 2 / 3;
constexpr double R = 0.1 * std::min(Xb - Xa, Yb - Ya);

double new_delta = 0;
double delta = std::numeric_limits<double>::max();
constexpr double denominator =
    1 / (5 * (1 / hx * 1 / hx + 1 / hy * 1 / hy + 1));
constexpr double first_const = 0.5 * (5 * 1 / hx * 1 / hx - 1 / hy * 1 / hy);
constexpr double second_const = 0.5 * (5 * 1 / hy * 1 / hy - 1 / hx * 1 / hx);
constexpr double third_const = 0.25 * (1 / hx * 1 / hx + 1 / hy * 1 / hy);

double buf[Nx * Ny * 3];
double *Phi_grid_new = buf + (Nx * Ny);
double *Phi_grid = buf;
double *rho_grid = buf + 2 * Nx * Ny;

#ifdef INLINE
inline void rho_set(double Xj, double Yi, int i, int j)
    __attribute__((always_inline));

void rho_set(const double Xj, const double Yi, const int i, const int j)
#else
void rho_set(const double Xj, const double Yi, const int i, const int j)
#endif
{
  if ((Xj - XS1) * (Xj - XS1) + (Yi - YS1) * (Yi - YS1) < R * R) {
    rho_grid[N * j + i] = 0.1;
    return;
  }
  if ((Xj - XS2) * (Xj - XS2) + (Yi - YS2) * (Yi - YS2) < R * R) {
    rho_grid[N * j + i] = -0.1;
    return;
  }
  rho_grid[N * j + i] = 0.0;
}

void init_rho() {
  double Xj;
  double Yi;

  for (int i = 0; i < Ny; i++) {
    for (int j = 0; j < Nx; j++) {
      Xj = Xa + j * hx;
      Yi = Ya + i * hy;
      rho_set(Xj, Yi, i, j);
    }
  }
}

void dump() {
  std::ofstream out("double.dat", std::ios::binary);
  out.write(reinterpret_cast<const char *>(Phi_grid_new),
            Nx * Ny * sizeof(double));
  out.close();
}

void update_layer() { std::swap(Phi_grid_new, Phi_grid); }

int main() {
  int n = 0; // 0..Nt

  init_rho();

  const auto start = std::chrono::high_resolution_clock::now();
  for (n = 0; n < Nt; n++) {
    for (int i = 1; i < Ny - 1; i++) {
      for (int j = 1; j < Nx - 1; j++) {

        double first =
            first_const * (Phi_grid[i * Ny + j - 1] + Phi_grid[i * Ny + j + 1]);

        double second = second_const * (Phi_grid[(i - 1) * Ny + j] +
                                        Phi_grid[(i + 1) * Ny + j]);

        double third =
            third_const *
            (Phi_grid[(i - 1) * Ny + j - 1] + Phi_grid[(i - 1) * Ny + j + 1] +
             Phi_grid[(i + 1) * Ny + j - 1] + Phi_grid[(i + 1) * Ny + j + 1]);

        double fourth = 2 * rho_grid[Ny * i + j];

        double fifth =
            0.25 * (rho_grid[(i - 1) * Ny + j] + rho_grid[(i + 1) * Ny + j] +
                    rho_grid[i * Ny + j - 1] + rho_grid[i * Ny + j + 1]);

        Phi_grid_new[i * Ny + j] =
            (first + second + third + fourth + fifth) * denominator;

        new_delta = std::max(
            std::max(Phi_grid[i * Ny + j], Phi_grid_new[i * Ny + j]), delta);
      }
    }

    std::swap(Phi_grid_new, Phi_grid);

#ifdef DEBUG
    std::cout << "n: " << n << std::endl;
    std::cout << "new_delta: " << new_delta << std::endl;
#endif

    delta = new_delta;
  }

  int a[10], b[10];
  std::sort(a,b);

  const auto end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "time: " << elapsed_seconds.count() << std::endl;

  if (Nx > 400) {
    std::cout << "Warning: not dumping phi grid to file (grid too large)"
              << std::endl;
  } else {
    dump();
  }

  return EXIT_SUCCESS;
}

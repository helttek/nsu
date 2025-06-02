#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <sched.h>
#include <unistd.h>
#include <vector>

#define GNU_SOURCE
#define INLINE
/*#define DEBUG*/

// optimizations
#define SET_CPU
#define CACHE_LOCALITY

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
double first;
double second;
double third;
double fourth;
double fifth;

#ifdef CACHE_LOCALITY
double buf[Nx * Ny * 3];
double *Phi_grid_new = buf + (Nx * Ny);
double *Phi_grid = buf;
double *rho_grid = buf + 2 * Nx * Ny;
#else
std::vector<double> Phi_grid_new(Nx *Ny);
std::vector<double> Phi_grid(Nx *Ny);
std::vector<double> rho_grid(Nx *Ny);
#endif

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
#ifdef CACHE_LOCALITY
  out.write(reinterpret_cast<const char *>(Phi_grid_new),
            Nx * Ny * sizeof(double));
#else
  out.write(reinterpret_cast<const char *>(Phi_grid_new.data()),
            Nx * Ny * sizeof(double));
#endif
  out.close();
}

void set_cpu() {
  cpu_set_t cpu_set;
  CPU_ZERO(&cpu_set);
  CPU_SET(2, &cpu_set);
  pid_t pid = getpid();
  if (sched_setaffinity(pid, sizeof(cpu_set_t), &cpu_set) == -1) {
    perror("sched_setaffinity");
  }
}

#ifdef INLINE
inline void calc_Phi(int i, int j) __attribute__((always_inline));

void calc_Phi(const int i, const int j)
#else
void calc_Phi(const int i, const int j)
#endif
{
  // first summand
  first = first_const * (Phi_grid[i * Ny + j - 1] + Phi_grid[i * Ny + j + 1]);

  // second summand
  second =
      second_const * (Phi_grid[(i - 1) * Ny + j] + Phi_grid[(i + 1) * Ny + j]);

  // third summand
  third = third_const *
          (Phi_grid[(i - 1) * Ny + j - 1] + Phi_grid[(i - 1) * Ny + j + 1] +
           Phi_grid[(i + 1) * Ny + j - 1] + Phi_grid[(i + 1) * Ny + j + 1]);

  // fourth summand
  fourth = 2 * rho_grid[Ny * i + j];

  // fifth summand
  fifth = 0.25 * (rho_grid[(i - 1) * Ny + j] + rho_grid[(i + 1) * Ny + j] +
                  rho_grid[i * Ny + j - 1] + rho_grid[i * Ny + j + 1]);

  // sum them all up
  Phi_grid_new[i * Ny + j] =
      (first + second + third + fourth + fifth) * denominator;

  // max(delta, max(old, new))
  new_delta =
      std::max(std::max(Phi_grid[i * Ny + j], Phi_grid_new[i * Ny + j]), delta);
}

#ifdef INLINE
inline void update_layer() __attribute__((always_inline));

void update_layer()
#else
void update_layer()
#endif
{
#ifdef CACHE_LOCALITY
  std::swap(Phi_grid_new, Phi_grid);
#else
  Phi_grid.swap(Phi_grid_new);
#endif
}
int main() {
  int n = 0; // 0..Nt

#ifdef SET_CPU
  set_cpu();
#endif

  init_rho();

  const auto start = std::chrono::high_resolution_clock::now();
  for (n = 0; n < Nt; n++) {
    for (int i = 1; i < Ny - 1; i++) {
      for (int j = 1; j < Nx - 1; j++) {
        calc_Phi(i, j);
      }
    }
    update_layer();

#ifdef DEBUG
    std::cout << "n: " << n << std::endl;
    std::cout << "new_delta: " << new_delta << std::endl;
#endif

    /*if (new_delta >= delta)*/
    /*{*/
    /*    std::cout << "ERROR: Delta is not decreasing." << std::endl;*/
    /*    std::cout << "delta: " << delta << std::endl;*/
    /*    std::cout << "new_delta: " << new_delta << std::endl;*/
    /*    break;*/
    /*} */
    delta = new_delta;
  }

  const auto end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "time: " << elapsed_seconds.count() << std::endl;

  dump();

  return EXIT_SUCCESS;
}

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <immintrin.h>
#include <iostream>
#include <limits>
#include <mm_malloc.h>
#include <sched.h>
#include <unistd.h>

#define GNU_SOURCE
#define INLINE
// #define DEBUG

constexpr int Nx = 8000;
constexpr int Ny = 8000;
constexpr int N = Nx; // N==Nx==Ny
constexpr int Nt = 100;

static_assert(Nx >= 5, "Nx must be at least 5 for the inner loop boundaries.");

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
constexpr double hx2_inv = 1.0 / (hx * hx);
constexpr double hy2_inv = 1.0 / (hy * hy);
constexpr double denominator_orig = 1.0 / (5.0 * (hx2_inv + hy2_inv + 1.0));
constexpr double first_const_orig = 0.5 * (5.0 * hx2_inv - hy2_inv);
constexpr double second_const_orig = 0.5 * (5.0 * hy2_inv - hx2_inv);
constexpr double third_const_orig = 0.25 * (hx2_inv + hy2_inv);
constexpr double fourth_const_orig = 2.0;
constexpr double fifth_const_orig = 0.25;

constexpr size_t alignment = 32; // avx2 - 256 bits
double *Phi_grid = nullptr;
double *Phi_grid_new = nullptr;
double *rho_grid = nullptr;

#ifdef INLINE
inline void rho_set(double Xj, double Yi, int i, int j)
    __attribute__((always_inline));
#endif
void rho_set(const double Xj, const double Yi, const int i, const int j) {
  if ((Xj - XS1) * (Xj - XS1) + (Yi - YS1) * (Yi - YS1) < R * R) {
    rho_grid[j * N + i] = 0.1;
    return;
  }
  if ((Xj - XS2) * (Xj - XS2) + (Yi - YS2) * (Yi - YS2) < R * R) {
    rho_grid[j * N + i] = -0.1;
    return;
  }
  rho_grid[j * N + i] = 0.0;
}

void init_rho() {
  double Xj;
  double Yi;

  for (int i = 0; i < Ny; i++) {
    for (int j = 0; j < Nx; j++) {
      Yi = Ya + i * hy;
      Xj = Xa + j * hx;
      rho_set(Xj, Yi, i, j);
    }
  }
}

/*
TODO:
-
*/

void dump() {
  std::ofstream out("double.dat", std::ios::binary);
  out.write(reinterpret_cast<const char *>(Phi_grid), Nx * Ny * sizeof(double));
  out.close();
}

inline double hmax_double(__m256d x) {
  __m128d low = _mm256_castpd256_pd128(x);
  __m128d high = _mm256_extractf128_pd(x, 1);
  __m128d max_low_high = _mm_max_pd(low, high);
  __m128d max_shuffled = _mm_shuffle_pd(max_low_high, max_low_high, 1);
  __m128d max_final = _mm_max_pd(max_low_high, max_shuffled);
  return _mm_cvtsd_f64(max_final);
}
double process_timestep_unaligned() {
  double current_max_delta = -std::numeric_limits<double>::max();
  const __m256d v_denom = _mm256_set1_pd(denominator_orig);
  const __m256d v_c1_den =
      _mm256_mul_pd(_mm256_set1_pd(first_const_orig), v_denom);
  const __m256d v_c2_den =
      _mm256_mul_pd(_mm256_set1_pd(second_const_orig), v_denom);
  const __m256d v_c3_den =
      _mm256_mul_pd(_mm256_set1_pd(third_const_orig), v_denom);
  const __m256d v_c4_den =
      _mm256_mul_pd(_mm256_set1_pd(fourth_const_orig), v_denom);
  const __m256d v_c5_den =
      _mm256_mul_pd(_mm256_set1_pd(fifth_const_orig), v_denom);

  for (int i = 1; i < Ny - 1; ++i) {
    size_t row_offset_i = static_cast<size_t>(i) * N;
    size_t row_offset_im1 = static_cast<size_t>(i - 1) * N;
    size_t row_offset_ip1 = static_cast<size_t>(i + 1) * N;

    double *phi_row_i = Phi_grid + row_offset_i;
    double *phi_row_im1 = Phi_grid + row_offset_im1;
    double *phi_row_ip1 = Phi_grid + row_offset_ip1;
    double *rho_row_i = rho_grid + row_offset_i;
    double *rho_row_im1 = rho_grid + row_offset_im1;
    double *rho_row_ip1 = rho_grid + row_offset_ip1;
    double *phi_new_row_i = Phi_grid_new + row_offset_i;

    int j = 1;
    int limit_j = Nx - 1 - 4;
    for (; j <= limit_j; j += 4) {
      __m256d phi_im1_jm1 = _mm256_loadu_pd(phi_row_im1 + j - 1);
      __m256d phi_im1_j = _mm256_loadu_pd(phi_row_im1 + j);
      __m256d phi_im1_jp1 = _mm256_loadu_pd(phi_row_im1 + j + 1);
      __m256d phi_i_jm1 = _mm256_loadu_pd(phi_row_i + j - 1);
      __m256d phi_i_j = _mm256_loadu_pd(phi_row_i + j);
      __m256d phi_i_jp1 = _mm256_loadu_pd(phi_row_i + j + 1);
      __m256d phi_ip1_jm1 = _mm256_loadu_pd(phi_row_ip1 + j - 1);
      __m256d phi_ip1_j = _mm256_loadu_pd(phi_row_ip1 + j);
      __m256d phi_ip1_jp1 = _mm256_loadu_pd(phi_row_ip1 + j + 1);
      __m256d rho_im1_j = _mm256_loadu_pd(rho_row_im1 + j);
      __m256d rho_i_jm1 = _mm256_loadu_pd(rho_row_i + j - 1);
      __m256d rho_i_j = _mm256_loadu_pd(rho_row_i + j);
      __m256d rho_i_jp1 = _mm256_loadu_pd(rho_row_i + j + 1);
      __m256d rho_ip1_j = _mm256_loadu_pd(rho_row_ip1 + j);

      __m256d term1_sum = _mm256_add_pd(phi_i_jm1, phi_i_jp1);
      __m256d result = _mm256_mul_pd(v_c1_den, term1_sum);
      __m256d term2_sum = _mm256_add_pd(phi_im1_j, phi_ip1_j);
      result = _mm256_fmadd_pd(v_c2_den, term2_sum, result);
      __m256d term3_sum1 = _mm256_add_pd(phi_im1_jm1, phi_im1_jp1);
      __m256d term3_sum2 = _mm256_add_pd(phi_ip1_jm1, phi_ip1_jp1);
      __m256d term3_sum = _mm256_add_pd(term3_sum1, term3_sum2);
      result = _mm256_fmadd_pd(v_c3_den, term3_sum, result);
      result = _mm256_fmadd_pd(v_c4_den, rho_i_j, result);
      __m256d term5_sum1 = _mm256_add_pd(rho_im1_j, rho_ip1_j);
      __m256d term5_sum2 = _mm256_add_pd(rho_i_jm1, rho_i_jp1);
      __m256d term5_sum = _mm256_add_pd(term5_sum1, term5_sum2);
      result = _mm256_fmadd_pd(v_c5_den, term5_sum, result);

      _mm256_storeu_pd(phi_new_row_i + j, result);

      __m256d current_phi_max = _mm256_max_pd(phi_i_j, result);
      double max_in_vector = hmax_double(current_phi_max);
      current_max_delta = std::max(current_max_delta, max_in_vector);
    }

    int end_j = Nx - 1;
    for (; j < end_j; ++j) {
      double first = first_const_orig * (phi_row_i[j - 1] + phi_row_i[j + 1]);
      double second = second_const_orig * (phi_row_im1[j] + phi_row_ip1[j]);
      double third =
          third_const_orig * (phi_row_im1[j - 1] + phi_row_im1[j + 1] +
                              phi_row_ip1[j - 1] + phi_row_ip1[j + 1]);
      double fourth = fourth_const_orig * rho_row_i[j];
      double fifth = fifth_const_orig * (rho_row_im1[j] + rho_row_ip1[j] +
                                         rho_row_i[j - 1] + rho_row_i[j + 1]);

      double phi_new_val =
          (first + second + third + fourth + fifth) * denominator_orig;
      phi_new_row_i[j] = phi_new_val;
      current_max_delta =
          std::max(std::max(phi_row_i[j], phi_new_val), current_max_delta);
    }
  }
  return current_max_delta;
}

int main() {
  size_t array_elements = static_cast<size_t>(Nx) * Ny;
  size_t array_bytes = array_elements * sizeof(double);

  Phi_grid = (double *)_mm_malloc(array_bytes, alignment);
  Phi_grid_new = (double *)_mm_malloc(array_bytes, alignment);
  rho_grid = (double *)_mm_malloc(array_bytes, alignment);

  if (!Phi_grid || !Phi_grid_new || !rho_grid) {
    std::cerr << "Failed to allocate memory" << std::endl;
    if (Phi_grid)
      _mm_free(Phi_grid);
    if (Phi_grid_new)
      _mm_free(Phi_grid_new);
    if (rho_grid)
      _mm_free(rho_grid);
    return EXIT_FAILURE;
  }

  int n = 0;

  init_rho();

  const auto start = std::chrono::high_resolution_clock::now();

  for (n = 0; n < Nt; n++) {
    double max_delta_this_step = process_timestep_unaligned();

    std::swap(Phi_grid_new, Phi_grid);

#ifdef DEBUG
    std::cout << "n: " << n << " max_delta: " << max_delta_this_step
              << std::endl;
#endif
  }

  const auto end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "time: " << elapsed_seconds.count() << std::endl;

  if (Nx > 400) {
    std::cout << "Warning: not dumping phi grid to file (grid too large)"
              << std::endl;
  } else {
    dump();
  }

  _mm_free(Phi_grid);
  _mm_free(Phi_grid_new);
  _mm_free(rho_grid);

  return EXIT_SUCCESS;
}
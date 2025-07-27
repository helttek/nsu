#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <immintrin.h>
#include <iostream>
#include <mm_malloc.h>
#include <stdexcept>
#include <string>
#include <vector>

#define GNU_SOURCE
#define INLINE inline
// #define DEBUG
// #define COMPILER

constexpr int Nx = 8000;
constexpr int Ny = 8000;
constexpr int N = Nx;
constexpr int Nt = 100;

int TEMPORAL_BLOCK_SIZE = 29;

static_assert(Nx >= 5, "Nx must be at least 5 for the inner loop boundaries.");

constexpr size_t alignment = 32; // avx2 - 256 bits / 8 bytes/double = 4 doubles
static_assert((Nx * sizeof(double)) % alignment == 0,
              "Row size in bytes (Nx * sizeof(double)) must be multiple of "
              "alignment for aligned loads on rows i-1, i+1");

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

constexpr double hx2_inv = 1.0 / (hx * hx);
constexpr double hy2_inv = 1.0 / (hy * hy);
constexpr double denominator_orig = 1.0 / (5.0 * (hx2_inv + hy2_inv + 1.0));
constexpr double first_const_orig = 0.5 * (5.0 * hx2_inv - hy2_inv);
constexpr double second_const_orig = 0.5 * (5.0 * hy2_inv - hx2_inv);
constexpr double third_const_orig = 0.25 * (hx2_inv + hy2_inv);
constexpr double fourth_const_orig = 2.0;
constexpr double fifth_const_orig = 0.25;

std::vector<double *> grids;
double *rho_grid = nullptr;

#ifdef INLINE
inline void rho_set(double Xj, double Yi, int i, int j)
    __attribute__((always_inline));
#endif
void rho_set(const double Xj, const double Yi, const int i, const int j) {
  size_t index = static_cast<size_t>(i) * Nx + j;
  if ((Xj - XS1) * (Xj - XS1) + (Yi - YS1) * (Yi - YS1) < R * R) {
    rho_grid[index] = 0.1;
    return;
  }
  if ((Xj - XS2) * (Xj - XS2) + (Yi - YS2) * (Yi - YS2) < R * R) {
    rho_grid[index] = -0.1;
    return;
  }
  rho_grid[index] = 0.0;
}

void init_rho() {
  double Xj;
  double Yi;

  for (int i = 0; i < Ny; i++) {
    Yi = Ya + i * hy;
    for (int j = 0; j < Nx; j++) {
      Xj = Xa + j * hx;
      rho_set(Xj, Yi, i, j);
    }
  }
}

void dump(double *grid) {
  std::ofstream out("double.dat", std::ios::binary);
  if (!out) {
    std::cerr << "Error: Cannot open double.dat for writing." << std::endl;
    return;
  }
  out.write(reinterpret_cast<const char *>(grid),
            static_cast<size_t>(Nx) * Ny * sizeof(double));
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

inline __m256d load_minus_one(const __m256d &v_prev, const __m256d &v_curr) {
  __m256d blended = _mm256_blend_pd(v_prev, v_curr, 0b0111);
  return _mm256_permute4x64_pd(blended, _MM_SHUFFLE(2, 1, 0, 3));
}

inline __m256d load_plus_one(const __m256d &v_curr, const __m256d &v_next) {
  __m256d blended = _mm256_blend_pd(v_curr, v_next, 0b1000);
  blended = _mm256_blend_pd(v_curr, v_next, 0b0001);
  return _mm256_permute4x64_pd(blended, _MM_SHUFFLE(0, 3, 2, 1));
}

constexpr int VEC_SIZE = 4;

// Функция для вычисления одного временного шага (скалярная версия)
inline double compute_point_scalar(double *phi_grid, double *rho_grid, int i,
                                   int j) {
  size_t row_offset_i = static_cast<size_t>(i) * Nx;
  size_t row_offset_im1 = static_cast<size_t>(i - 1) * Nx;
  size_t row_offset_ip1 = static_cast<size_t>(i + 1) * Nx;

  double first = first_const_orig * (phi_grid[row_offset_i + j - 1] +
                                     phi_grid[row_offset_i + j + 1]);
  double second = second_const_orig *
                  (phi_grid[row_offset_im1 + j] + phi_grid[row_offset_ip1 + j]);
  double third =
      third_const_orig *
      (phi_grid[row_offset_im1 + j - 1] + phi_grid[row_offset_im1 + j + 1] +
       phi_grid[row_offset_ip1 + j - 1] + phi_grid[row_offset_ip1 + j + 1]);
  double fourth = fourth_const_orig * rho_grid[row_offset_i + j];
  double fifth =
      fifth_const_orig *
      (rho_grid[row_offset_im1 + j] + rho_grid[row_offset_ip1 + j] +
       rho_grid[row_offset_i + j - 1] + rho_grid[row_offset_i + j + 1]);

  return (first + second + third + fourth + fifth) * denominator_orig;
}

double process_timestep_aligned(int i, double *Phi_grid, double *Phi_grid_new) {
  double current_max_delta = 0.0;
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
  const __m256d v_sign_mask = _mm256_set1_pd(-0.0);

  size_t row_offset_i = static_cast<size_t>(i) * Nx;
  size_t row_offset_im1 = static_cast<size_t>(i - 1) * Nx;
  size_t row_offset_ip1 = static_cast<size_t>(i + 1) * Nx;

  double *phi_row_i = Phi_grid + row_offset_i;
  double *phi_row_im1 = Phi_grid + row_offset_im1;
  double *phi_row_ip1 = Phi_grid + row_offset_ip1;
  double *rho_row_i = rho_grid + row_offset_i;
  double *rho_row_im1 = rho_grid + row_offset_im1;
  double *rho_row_ip1 = rho_grid + row_offset_ip1;
  double *phi_new_row_i = Phi_grid_new + row_offset_i;

  int j = 1;
  const int end_j = Nx - 1;

  int scalar_end_j = std::min(VEC_SIZE, end_j);
  for (; j < scalar_end_j; ++j) {
    double first = first_const_orig * (phi_row_i[j - 1] + phi_row_i[j + 1]);
    double second = second_const_orig * (phi_row_im1[j] + phi_row_ip1[j]);
    double third = third_const_orig * (phi_row_im1[j - 1] + phi_row_im1[j + 1] +
                                       phi_row_ip1[j - 1] + phi_row_ip1[j + 1]);
    double fourth = fourth_const_orig * rho_row_i[j];
    double fifth = fifth_const_orig * (rho_row_im1[j] + rho_row_ip1[j] +
                                       rho_row_i[j - 1] + rho_row_i[j + 1]);

    double phi_new_val =
        (first + second + third + fourth + fifth) * denominator_orig;
    phi_new_row_i[j] = phi_new_val;

    current_max_delta =
        std::max(current_max_delta, std::abs(phi_new_val - phi_row_i[j]));
  }

  const int limit_j = end_j - (VEC_SIZE - 1);

  if (j < limit_j) {
    __m256d phi_im1_curr = _mm256_load_pd(phi_row_im1 + j);
    __m256d phi_i_curr = _mm256_load_pd(phi_row_i + j);
    __m256d phi_ip1_curr = _mm256_load_pd(phi_row_ip1 + j);
    __m256d rho_im1_j = _mm256_load_pd(rho_row_im1 + j);
    __m256d rho_ip1_j = _mm256_load_pd(rho_row_ip1 + j);
    __m256d rho_i_curr = _mm256_load_pd(rho_row_i + j);

    __m256d phi_im1_prev = _mm256_load_pd(phi_row_im1 + j - VEC_SIZE);
    __m256d phi_i_prev = _mm256_load_pd(phi_row_i + j - VEC_SIZE);
    __m256d phi_ip1_prev = _mm256_load_pd(phi_row_ip1 + j - VEC_SIZE);
    __m256d rho_i_prev = _mm256_load_pd(rho_row_i + j - VEC_SIZE);

    __m256d phi_im1_next = _mm256_load_pd(phi_row_im1 + j + VEC_SIZE);
    __m256d phi_i_next = _mm256_load_pd(phi_row_i + j + VEC_SIZE);
    __m256d phi_ip1_next = _mm256_load_pd(phi_row_ip1 + j + VEC_SIZE);
    __m256d rho_i_next = _mm256_load_pd(rho_row_i + j + VEC_SIZE);

    for (; j < limit_j; j += VEC_SIZE) {
      __m256d phi_im1_jm1 = load_minus_one(phi_im1_prev, phi_im1_curr);
      __m256d phi_im1_jp1 = load_plus_one(phi_im1_curr, phi_im1_next);
      __m256d phi_i_jm1 = load_minus_one(phi_i_prev, phi_i_curr);
      __m256d phi_i_jp1 = load_plus_one(phi_i_curr, phi_i_next);
      __m256d phi_ip1_jm1 = load_minus_one(phi_ip1_prev, phi_ip1_curr);
      __m256d phi_ip1_jp1 = load_plus_one(phi_ip1_curr, phi_ip1_next);
      __m256d rho_i_jm1 = load_minus_one(rho_i_prev, rho_i_curr);
      __m256d rho_i_jp1 = load_plus_one(rho_i_curr, rho_i_next);

      __m256d term1_sum = _mm256_add_pd(phi_i_jm1, phi_i_jp1);
      __m256d result = _mm256_mul_pd(v_c1_den, term1_sum);

      __m256d term2_sum = _mm256_add_pd(phi_im1_curr, phi_ip1_curr);
      result = _mm256_fmadd_pd(v_c2_den, term2_sum, result);

      __m256d term3_sum1 = _mm256_add_pd(phi_im1_jm1, phi_im1_jp1);
      __m256d term3_sum2 = _mm256_add_pd(phi_ip1_jm1, phi_ip1_jp1);
      __m256d term3_sum = _mm256_add_pd(term3_sum1, term3_sum2);
      result = _mm256_fmadd_pd(v_c3_den, term3_sum, result);

      result = _mm256_fmadd_pd(v_c4_den, rho_i_curr, result);

      __m256d term5_sum1 = _mm256_add_pd(rho_im1_j, rho_ip1_j);
      __m256d term5_sum2 = _mm256_add_pd(rho_i_jm1, rho_i_jp1);
      __m256d term5_sum = _mm256_add_pd(term5_sum1, term5_sum2);
      result = _mm256_fmadd_pd(v_c5_den, term5_sum, result);

      _mm256_store_pd(phi_new_row_i + j, result);

      __m256d diff = _mm256_sub_pd(result, phi_i_curr);
      __m256d abs_diff = _mm256_andnot_pd(v_sign_mask, diff);
      double max_in_vector = hmax_double(abs_diff);
      current_max_delta = std::max(current_max_delta, max_in_vector);

      phi_im1_prev = phi_im1_curr;
      phi_i_prev = phi_i_curr;
      phi_ip1_prev = phi_ip1_curr;
      rho_i_prev = rho_i_curr;
      phi_im1_curr = phi_im1_next;
      phi_i_curr = phi_i_next;
      phi_ip1_curr = phi_ip1_next;
      rho_i_curr = rho_i_next;

      if (j + VEC_SIZE < limit_j) {
        phi_im1_next = _mm256_load_pd(phi_row_im1 + j + 2 * VEC_SIZE);
        phi_i_next = _mm256_load_pd(phi_row_i + j + 2 * VEC_SIZE);
        phi_ip1_next = _mm256_load_pd(phi_row_ip1 + j + 2 * VEC_SIZE);
        rho_i_next = _mm256_load_pd(rho_row_i + j + 2 * VEC_SIZE);
      }
      rho_im1_j = _mm256_load_pd(rho_row_im1 + j + VEC_SIZE);
      rho_ip1_j = _mm256_load_pd(rho_row_ip1 + j + VEC_SIZE);
    }
  }

  for (; j < end_j; ++j) {
    double first = first_const_orig * (phi_row_i[j - 1] + phi_row_i[j + 1]);
    double second = second_const_orig * (phi_row_im1[j] + phi_row_ip1[j]);
    double third = third_const_orig * (phi_row_im1[j - 1] + phi_row_im1[j + 1] +
                                       phi_row_ip1[j - 1] + phi_row_ip1[j + 1]);
    double fourth = fourth_const_orig * rho_row_i[j];
    double fifth = fifth_const_orig * (rho_row_im1[j] + rho_row_ip1[j] +
                                       rho_row_i[j - 1] + rho_row_i[j + 1]);

    double phi_new_val =
        (first + second + third + fourth + fifth) * denominator_orig;
    phi_new_row_i[j] = phi_new_val;

    current_max_delta =
        std::max(current_max_delta, std::abs(phi_new_val - phi_row_i[j]));
  }
  return current_max_delta;
}

double process_temporal_block() {
  double current_max_delta = 0.0;

  for (int block_start = 0; block_start < Nt;
       block_start += TEMPORAL_BLOCK_SIZE) {
    int actual_block_size = std::min(TEMPORAL_BLOCK_SIZE, Nt - block_start);

    if (actual_block_size == 1) {
      for (int i = 1; i < Ny - 1; ++i) {
#ifdef COMPILER
        for (int j = 1; j < Nx - 1; ++j) {
          double new_val = compute_point_scalar(grids[0], rho_grid, i, j);
          grids[1][static_cast<size_t>(i) * Nx + j] = new_val;
          current_max_delta = std::max(
              current_max_delta,
              std::abs(new_val - grids[0][static_cast<size_t>(i) * Nx + j]));
        }
#else
        process_timestep_aligned(1, grids[0], grids[1]);
#endif
      }
      std::swap(grids[0], grids[1]);
      continue;
    }

    if (actual_block_size >= 2) {
      int i = 1;
#ifdef COMPILER
      for (int j = 1; j < Nx - 1; ++j) {
        double new_val = compute_point_scalar(grids[0], rho_grid, i, j);
        grids[1][static_cast<size_t>(i) * Nx + j] = new_val;
        current_max_delta = std::max(
            current_max_delta,
            std::abs(new_val - grids[0][static_cast<size_t>(i) * Nx + j]));
      }
#else
      process_timestep_aligned(i, grids[0], grids[1]);
#endif

      for (i = 2; i < Ny - 1; ++i) {
#ifdef COMPILER
        for (int j = 1; j < Nx - 1; ++j) {
          double new_val = compute_point_scalar(grids[0], rho_grid, i, j);
          grids[1][static_cast<size_t>(i) * Nx + j] = new_val;
          current_max_delta = std::max(
              current_max_delta,
              std::abs(new_val - grids[0][static_cast<size_t>(i) * Nx + j]));
        }
#else
        process_timestep_aligned(i, grids[0], grids[1]);
#endif

        // Теперь можем вычислить предыдущую строку для следующих итераций блока
        for (int time_step = 1; time_step < actual_block_size; ++time_step) {
          int src_grid = time_step % 2;
          int dst_grid = 1 - src_grid;

          // Проверяем, что у нас есть достаточно вычисленных строк
          if (i - time_step >= 1) {
#ifdef COMPILER
            for (int j = 1; j < Nx - 1; ++j) {
              double new_val = compute_point_scalar(grids[src_grid], rho_grid,
                                                    i - time_step, j);
              grids[dst_grid][static_cast<size_t>(i - time_step) * Nx + j] =
                  new_val;
              current_max_delta = std::max(
                  current_max_delta,
                  std::abs(new_val -
                           grids[src_grid]
                                [static_cast<size_t>(i - time_step) * Nx + j]));
            }
#else
            process_timestep_aligned(i - time_step, grids[src_grid],
                                     grids[src_grid & 1]);
#endif // COMPILER
          }
        }
      }

      // Обрабатываем последние строки для всех оставшихся временных шагов
      for (int time_step = 1; time_step < actual_block_size; ++time_step) {
        int src_grid = time_step % 2;
        int dst_grid = 1 - src_grid;

        int max_row = std::min(Ny - 1, Ny - time_step);
        for (int i = std::max(1, Ny - 1 - time_step + 1); i < max_row; ++i) {
#ifdef COMPILER
          for (int j = 1; j < Nx - 1; ++j) {
            double new_val =
                compute_point_scalar(grids[src_grid], rho_grid, i, j);
            grids[dst_grid][static_cast<size_t>(i) * Nx + j] = new_val;
            current_max_delta = std::max(
                current_max_delta,
                std::abs(new_val -
                         grids[src_grid][static_cast<size_t>(i) * Nx + j]));
          }
#else
          process_timestep_aligned(i, grids[src_grid], grids[src_grid & 1]);
#endif
        }
      }

      // Обновляем указатель на текущую сетку
      if (actual_block_size % 2 == 0) {
        // Четное количество итераций - данные остались в исходной сетке
      } else {
        // Нечетное количество итераций - данные в другой сетке
        std::swap(grids[0], grids[1]);
      }
    }
  }

  return current_max_delta;
}

int main(int argc, char **argv) {
  if (argc > 1) {
    try {
      TEMPORAL_BLOCK_SIZE = std::stoi(argv[1]);
    } catch (const std::invalid_argument e) {
      std::cout << "invalid argument in argv\n";
      return EXIT_FAILURE;
    }
  } else {
    TEMPORAL_BLOCK_SIZE = 29;
  }

  size_t array_elements = static_cast<size_t>(Nx) * Ny;
  size_t array_bytes = array_elements * sizeof(double);

  grids.resize(2);
  grids[0] = (double *)_mm_malloc(array_bytes, alignment);
  grids[1] = (double *)_mm_malloc(array_bytes, alignment);
  rho_grid = (double *)_mm_malloc(array_bytes, alignment);

  if (!grids[0] || !grids[1] || !rho_grid) {
    std::cerr << "Error: Failed to allocate aligned memory." << std::endl;
    if (grids[0])
      _mm_free(grids[0]);
    if (grids[1])
      _mm_free(grids[1]);
    if (rho_grid)
      _mm_free(rho_grid);
    return EXIT_FAILURE;
  }

  memset(grids[0], 0, array_bytes);
  memset(grids[1], 0, array_bytes);
  init_rho();

  const auto start = std::chrono::high_resolution_clock::now();

  double max_delta = process_temporal_block();

  const auto end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_seconds = end - start;

  std::cout << "time: " << elapsed_seconds.count() << std::endl;
  std::cout << "TEMPORAL_BLOCK_SIZE: " << TEMPORAL_BLOCK_SIZE << std::endl;

  if (static_cast<size_t>(Nx) * Ny > 400 * 400) {
    std::cout << "Warning: not dumping phi grid to file (grid too large)"
              << std::endl;
  } else {
    dump(grids[0]);
  }

  _mm_free(grids[0]);
  _mm_free(grids[1]);
  _mm_free(rho_grid);

  return EXIT_SUCCESS;
}

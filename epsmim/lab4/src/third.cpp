#include <algorithm>
#include <atomic>
#include <barrier>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <immintrin.h>
#include <iostream>
#include <memory>
#include <mm_malloc.h>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#define GNU_SOURCE
#define INLINE inline

#define COMPILER

constexpr int Nx = 8000;
constexpr int Ny = 8000;
constexpr int N = Nx;

constexpr int Nt = 100;

int TEMPORAL_BLOCK_SIZE = 29;
int NUM_THREADS = std::thread::hardware_concurrency();

static_assert(Nx >= 5, "Nx must be at least 5 for the inner loop boundaries.");

constexpr size_t alignment = 32;
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

std::vector<double *> grids;
double *rho_grid = nullptr;

std::atomic<double> global_max_delta(0.0);

std::vector<std::unique_ptr<std::atomic<int>>> boundary_row_progress;

struct alignas(64) ThreadSyncData {
  std::atomic<int> current_read_block;

  std::atomic<int> current_write_block;

  std::atomic<int> blocks_processed;
  std::atomic<double> max_delta;
  std::atomic<int> first_read_row;
  std::atomic<int> last_write_row;
  std::atomic<int> first_write_row;
  std::atomic<int> current_thread_idx;
};

std::vector<std::unique_ptr<ThreadSyncData>> thread_sync;

constexpr double denominator =
    1 / (5 * (1 / hx * 1 / hx + 1 / hy * 1 / hy + 1));
constexpr double first_const = 0.5 * (5 * 1 / hx * 1 / hx - 1 / hy * 1 / hy);
constexpr double second_const = 0.5 * (5 * 1 / hy * 1 / hy - 1 / hx * 1 / hx);
constexpr double third_const = 0.25 * (1 / hx * 1 / hx + 1 / hy * 1 / hy);

INLINE double ro(double x, double y) {
  if (x >= XS1 && x <= XS2 && y >= YS2 && y <= YS1) {
    return 1.0;
  }
  return 0.0;
}

void init_rho() {
  for (int i = 0; i < Nx; ++i) {
    for (int j = 0; j < Ny; ++j) {
      rho_grid[i * Ny + j] = ro(Xa + i * hx, Ya + j * hy);
    }
  }
}

void initialize_rho_thread_part(int start_row_rho, int end_row_rho) {
  for (int i = start_row_rho; i < end_row_rho; ++i) {
    for (int j = 0; j < Ny; ++j) {
      rho_grid[i * Ny + j] = ro(Xa + i * hx, Ya + j * hy);
    }
  }
}

void thread_worker_pairwise(int thread_id, int start_row_for_compute,
                            int end_row_for_compute) {

  int rows_per_thread_init = Ny / NUM_THREADS;
  int start_row_init = thread_id * rows_per_thread_init;
  int end_row_init = (thread_id == NUM_THREADS - 1)
                         ? Ny
                         : (thread_id + 1) * rows_per_thread_init;
  initialize_rho_thread_part(start_row_init, end_row_init);

  thread_sync[thread_id]->current_read_block.store(0);
  thread_sync[thread_id]->current_write_block.store(0);
  thread_sync[thread_id]->blocks_processed.store(0);
  thread_sync[thread_id]->max_delta.store(0.0);
  thread_sync[thread_id]->first_read_row.store(start_row_for_compute);
  thread_sync[thread_id]->last_write_row.store(end_row_for_compute);
  thread_sync[thread_id]->first_write_row.store(start_row_for_compute);
  thread_sync[thread_id]->current_thread_idx.store(thread_id);

  for (int block_start = 0; block_start < Nt;
       block_start += TEMPORAL_BLOCK_SIZE) {
    int actual_block_size = std::min(TEMPORAL_BLOCK_SIZE, Nt - block_start);

    for (int t_offset = 0; t_offset < actual_block_size; ++t_offset) {
      double local_max_delta = 0.0;
      int current_global_iteration = block_start + t_offset;

      if (thread_id > 0) {
        while (boundary_row_progress[thread_id - 1]->load(
                   std::memory_order_acquire) < current_global_iteration) {
        }
      }

      for (int i = start_row_for_compute; i < end_row_for_compute; ++i) {
        if (i == 0 || i == Ny - 1)
          continue;
        for (int j = 1; j < Nx - 1; ++j) {

          double first =
              first_const * (grids[0][static_cast<size_t>(i) * Ny + j - 1] +
                             grids[0][static_cast<size_t>(i) * Ny + j + 1]);
          double second =
              second_const * (grids[0][static_cast<size_t>(i - 1) * Ny + j] +
                              grids[0][static_cast<size_t>(i + 1) * Ny + j]);
          double third =
              third_const * (grids[0][static_cast<size_t>(i - 1) * Ny + j - 1] +
                             grids[0][static_cast<size_t>(i - 1) * Ny + j + 1] +
                             grids[0][static_cast<size_t>(i + 1) * Ny + j - 1] +
                             grids[0][static_cast<size_t>(i + 1) * Ny + j + 1]);
          double fourth = 2 * rho_grid[static_cast<size_t>(i) * Ny + j];
          double fifth = 0.25 * (rho_grid[static_cast<size_t>(i - 1) * Ny + j] +
                                 rho_grid[static_cast<size_t>(i + 1) * Ny + j] +
                                 rho_grid[static_cast<size_t>(i) * Ny + j - 1] +
                                 rho_grid[static_cast<size_t>(i) * Ny + j + 1]);

          grids[1][static_cast<size_t>(i) * Ny + j] =
              (first + second + third + fourth + fifth) * denominator;

          local_max_delta =
              std::max(local_max_delta,
                       std::abs(grids[0][static_cast<size_t>(i) * Ny + j] -
                                grids[1][static_cast<size_t>(i) * Ny + j]));
        }
      }

      if (thread_id < NUM_THREADS - 1) {
        boundary_row_progress[thread_id]->store(current_global_iteration + 1,
                                                std::memory_order_release);
      }

      thread_sync[thread_id]->max_delta.store(
          std::max(
              thread_sync[thread_id]->max_delta.load(std::memory_order_relaxed),
              local_max_delta),
          std::memory_order_release);
    }

    thread_sync[thread_id]->current_write_block.store(
        block_start + actual_block_size, std::memory_order_release);
    thread_sync[thread_id]->blocks_processed.fetch_add(
        1, std::memory_order_release);

    if (thread_id == 0) {

      for (int i = 0; i < NUM_THREADS; ++i) {
        while (thread_sync[i]->current_write_block.load(
                   std::memory_order_acquire) <
               block_start + actual_block_size) {
        }
      }

      double current_block_global_delta = 0.0;
      for (int i = 0; i < NUM_THREADS; ++i) {
        current_block_global_delta =
            std::max(current_block_global_delta,
                     thread_sync[i]->max_delta.load(std::memory_order_relaxed));
        thread_sync[i]->max_delta.store(0.0, std::memory_order_release);
      }
      global_max_delta.store(
          std::max(global_max_delta.load(std::memory_order_relaxed),
                   current_block_global_delta),
          std::memory_order_release);

      std::swap(grids[0], grids[1]);

      if (global_max_delta.load(std::memory_order_relaxed) <
          std::numeric_limits<double>::epsilon()) {
        std::cout << "Converged (global) at iteration "
                  << block_start + actual_block_size << std::endl;
      }
    }
  }
}

double process_temporal_block_parallel_pairwise() {
  global_max_delta.store(0.0);

  thread_sync.clear();
  thread_sync.reserve(NUM_THREADS);
  for (int i = 0; i < NUM_THREADS; ++i) {
    thread_sync.push_back(std::make_unique<ThreadSyncData>());
  }

  std::vector<std::thread> threads;
  threads.reserve(NUM_THREADS);

  int rows_per_thread = (Ny - 2) / NUM_THREADS;
  int remaining_rows = (Ny - 2) % NUM_THREADS;

  int current_start_row = 1;

  for (int thread_id = 0; thread_id < NUM_THREADS; ++thread_id) {
    int thread_rows = rows_per_thread + (thread_id < remaining_rows ? 1 : 0);
    int start_row_for_compute = current_start_row;
    int end_row_for_compute = current_start_row + thread_rows;

    threads.emplace_back(thread_worker_pairwise, thread_id,
                         start_row_for_compute, end_row_for_compute);

    current_start_row = end_row_for_compute;
  }

  for (auto &thread : threads) {
    thread.join();
  }

  return global_max_delta.load();
}

double process_temporal_block_sequential() {
  double current_max_delta_seq = 0.0;

  for (int block_start = 0; block_start < Nt;
       block_start += TEMPORAL_BLOCK_SIZE) {
    int actual_block_size = std::min(TEMPORAL_BLOCK_SIZE, Nt - block_start);

    for (int t_offset = 0; t_offset < actual_block_size; ++t_offset) {
      current_max_delta_seq = 0.0;

      for (int i = 1; i < Ny - 1; ++i) {
        for (int j = 1; j < Nx - 1; ++j) {
          double first =
              first_const * (grids[0][static_cast<size_t>(i) * Ny + j - 1] +
                             grids[0][static_cast<size_t>(i) * Ny + j + 1]);

          double second =
              second_const * (grids[0][static_cast<size_t>(i - 1) * Ny + j] +
                              grids[0][static_cast<size_t>(i + 1) * Ny + j]);

          double third =
              third_const * (grids[0][static_cast<size_t>(i - 1) * Ny + j - 1] +
                             grids[0][static_cast<size_t>(i - 1) * Ny + j + 1] +
                             grids[0][static_cast<size_t>(i + 1) * Ny + j - 1] +
                             grids[0][static_cast<size_t>(i + 1) * Ny + j + 1]);

          double fourth = 2 * rho_grid[static_cast<size_t>(i) * Ny + j];

          double fifth = 0.25 * (rho_grid[static_cast<size_t>(i - 1) * Ny + j] +
                                 rho_grid[static_cast<size_t>(i + 1) * Ny + j] +
                                 rho_grid[static_cast<size_t>(i) * Ny + j - 1] +
                                 rho_grid[static_cast<size_t>(i) * Ny + j + 1]);

          grids[1][static_cast<size_t>(i) * Ny + j] =
              (first + second + third + fourth + fifth) * denominator;

          current_max_delta_seq =
              std::max(current_max_delta_seq,
                       std::abs(grids[0][static_cast<size_t>(i) * Ny + j] -
                                grids[1][static_cast<size_t>(i) * Ny + j]));
        }
      }

      std::swap(grids[0], grids[1]);

      if (current_max_delta_seq < std::numeric_limits<double>::epsilon()) {
        std::cout << "Converged sequentially at iteration "
                  << block_start + t_offset << std::endl;
        return current_max_delta_seq;
      }
    }
  }
  return current_max_delta_seq;
}

int main(int argc, char *argv[]) {

  if (argc > 1) {
    try {
      int threads_arg = std::stoi(argv[1]);
      if (threads_arg > 0) {
        NUM_THREADS = threads_arg;
      } else {
        std::cout << "Invalid thread count in argv[1], using default "
                     "hardware_concurrency."
                  << std::endl;
      }
    } catch (const std::invalid_argument &e) {
      std::cout << "Invalid thread count in argv[1], using default "
                   "hardware_concurrency."
                << std::endl;
    }
  }

  if (argc > 2) {
    try {
      int block_size_arg = std::stoi(argv[2]);
      if (block_size_arg > 0) {
        TEMPORAL_BLOCK_SIZE = block_size_arg;
      } else {
        std::cout << "Invalid temporal block size in argv[2], using default."
                  << std::endl;
      }
    } catch (const std::invalid_argument &e) {
      std::cout << "Invalid temporal block size in argv[2], using default."
                << std::endl;
    }
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

  if (NUM_THREADS > 1) {
    boundary_row_progress.clear();
    boundary_row_progress.reserve(NUM_THREADS - 1);
    for (int i = 0; i < NUM_THREADS - 1; ++i) {
      boundary_row_progress.push_back(std::make_unique<std::atomic<int>>(-1));
    }
  }

  const auto start = std::chrono::high_resolution_clock::now();

  double max_delta;
  if (NUM_THREADS > 1) {
    max_delta = process_temporal_block_parallel_pairwise();
  } else {
    max_delta = process_temporal_block_sequential();
  }

  const auto end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_seconds = end - start;

  std::cout << "Time: " << elapsed_seconds.count() << " seconds" << std::endl;
  std::cout << "TEMPORAL_BLOCK_SIZE: " << TEMPORAL_BLOCK_SIZE << std::endl;
  std::cout << "NUM_THREADS: " << NUM_THREADS << std::endl;
  std::cout << "Max delta: " << max_delta << std::endl;

  _mm_free(grids[0]);
  _mm_free(grids[1]);
  _mm_free(rho_grid);

  return EXIT_SUCCESS;
}

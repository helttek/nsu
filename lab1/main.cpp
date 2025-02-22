#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <sched.h>
#include <unistd.h>

#define GNU_SOURCE

constexpr int Nx = 8000;
constexpr int Ny = 8000;
constexpr int N = Nx; //N==Nx==Ny
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

std::array<double, Nx * Ny> Phi_grid_new;
std::array<double, Nx * Ny> Phi_grid;
std::array<double, Nx * Ny> rho_grid;

void rho_set(const double Xj, const double Yi, const int i, const int j)
{
    if ((Xj - XS1) * (Xj - XS1) + (Yi - YS1) * (Yi - YS1) < R * R)
    {
        rho_grid[N * j + i] = 0.1;
        return;
    }
    if ((Xj - XS2) * (Xj - XS2) + (Yi - YS2) * (Yi - YS2) < R * R)
    {
        rho_grid[N * j + i] = -0.1;
        return;
    }
    rho_grid[N * j + i] = 0.0;
}

void init_rho()
{
    double Xj;
    double Yi;
    for (int i = 0; i < Ny; i++)
    {
        for (int j = 0; j < Nx; j++)
        {
            Xj = Xa + j * hx;
            Yi = Ya + i * hy;
            rho_set(Xj, Yi, i, j);
        }
    }
}

void dump()
{
    std::ofstream out("double.dat", std::ios::binary);
    out.write(reinterpret_cast<const char*>(Phi_grid_new.data()), Nx * Ny * sizeof(double));
    out.close();
}

void set_cpu()
{
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(0, &cpu_set);
    pid_t pid = getpid();
    if (sched_setaffinity(pid, sizeof(cpu_set_t), &cpu_set) == -1)
    {
        perror("sched_setaffinity");
    }
}

int main()
{
    int n = 0; //0..Nt

    set_cpu();

    double new_delta = 0;
    double delta = std::numeric_limits<double>::max();
    constexpr double denominator = 5 * (1 / hx * 1 / hx + 1 / hy * 1 / hy + 1);
    double first;
    double second;
    double third;
    double fourth;
    double fifth;

    init_rho();

    const auto start = std::chrono::high_resolution_clock::now();
    for (n = 0; n < Nt; n++)
    {
        for (int i = 1; i < Ny - 1; i++)
        {
            for (int j = 1; j < Nx - 1; j++)
            {
                //first summand
                first = 0.5 * (5 * 1 / hx * 1 / hx - 1 / hy * 1 / hy) * (Phi_grid[i * Ny + j - 1] + Phi_grid[i * Ny + j
                    + 1]);

                //second summand
                second = 0.5 * (5 * 1 / hy * 1 / hy - 1 / hx * 1 / hx) * (Phi_grid[(i - 1) * Ny + j] + Phi_grid[(i + 1)
                    * Ny + j]);

                //third summand
                third = 0.25 * (1 / hx * 1 / hx + 1 / hy * 1 / hy) * (Phi_grid[(i - 1) * Ny + j - 1] + Phi_grid[(i - 1)
                    * Ny + j + 1] + Phi_grid[(i + 1) * Ny + j - 1] + Phi_grid[(i + 1) * Ny + j + 1]);

                //fourth summand
                fourth = 2 * rho_grid[Ny * i + j];

                //fifth summand
                fifth = 0.25 * (rho_grid[(i - 1) * Ny + j] + rho_grid[(i + 1) * Ny + j] + rho_grid[i * Ny + j - 1] +
                    rho_grid[i * Ny + j + 1]);

                //sum them all up
                Phi_grid_new[i * Ny + j] = (first + second + third + fourth + fifth) / denominator;

                if (std::max(Phi_grid_new[i * Ny + j], Phi_grid[i * Ny + j]) > new_delta)
                {
                    new_delta = std::max(Phi_grid_new[i * Ny + j], Phi_grid[i * Ny + j]);
                }
            }
        }

        Phi_grid = Phi_grid_new;

        std::cout << "n: " << n << std::endl;
        std::cout << "new_delta: " << new_delta << std::endl;
        // if (new_delta >= delta)
        // {
        //     std::cout << "ERROR: Delta is not decreasing." << std::endl;
        //     std::cout << "delta: " << delta << std::endl;
        //     std::cout << "new_delta: " << new_delta << std::endl;
        //     break;
        // }
        delta = new_delta;
    }
    const auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "time: " << elapsed_seconds << std::endl;

    dump();

    return EXIT_SUCCESS;
}

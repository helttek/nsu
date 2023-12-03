cmake_minimum_required(VERSION 3.27)
project(cpu_cache)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-O1 -lm)

add_executable(cpu_cache main.cpp ../plots/pbPlots.cpp ../plots/pbPlots.hpp ../plots/supportLib.cpp ../plots/supportLib.hpp)

cmake_minimum_required(VERSION 3.27)
project(test0)

set(CMAKE_CXX_STANDARD 17)

add_executable(test0 main.cpp
#        tests/testsToRun.cpp
        bitarray.cpp
        bitarray.h)

add_subdirectory(tests)
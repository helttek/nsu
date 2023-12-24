cmake_minimum_required(VERSION 3.14)
project(inverse
        VERSION 1.0.0
        DESCRIPTION "Template for C++ library built with CMake"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "")

add_compile_options(-mfma)

add_executable(InversionTest
        inverse_matrix.cpp
        inverse_matrix.h
        inverse_blas.cpp
        inverse_blas.h
        inverse_fast.cpp
        inverse_fast.h
        main.cpp
)

find_package(BLAS REQUIRED)
#target_link_libraries(lab7 Eigen3::Eigen GTest::gtest GTest::gtest_main ${BLAS_LIBRARIES})

find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(GTest REQUIRED)
target_link_libraries(InversionTest Eigen3::Eigen gtest gtest_main ${BLAS_LIBRARIES})
link_libraries(libblas.a libopenblas.a)

#target_sources(InversionTest PRIVATE inversion_test.cpp)

#source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

#
#add_test(NAME InversionTest
#        COMMAND InversionTest)
#enable_testing()

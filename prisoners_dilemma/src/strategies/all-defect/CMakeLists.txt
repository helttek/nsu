cmake_minimum_required(VERSION 3.27)

project(all-defect-strategy)

add_library(all-defect SHARED src/all-defect.cpp)

target_include_directories(all-defect
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

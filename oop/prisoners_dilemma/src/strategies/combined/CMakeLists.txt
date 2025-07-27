cmake_minimum_required(VERSION 3.27)

project(combined_strategy)

add_library(combined SHARED src/combined.cpp)

target_include_directories(combined
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

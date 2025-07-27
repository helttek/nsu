cmake_minimum_required(VERSION 3.27)

project(random-strategy)

add_library(random SHARED src/random.cpp)

target_include_directories(random PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

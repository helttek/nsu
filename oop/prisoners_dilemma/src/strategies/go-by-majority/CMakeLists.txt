cmake_minimum_required(VERSION 3.27)

project(go-by-majority-strategy)

add_library(go-by-majority SHARED src/go-by-majority.cpp)

target_include_directories(go-by-majority
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

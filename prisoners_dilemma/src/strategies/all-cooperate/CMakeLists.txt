cmake_minimum_required(VERSION 3.27)

project(all_cooperate_strategy)

add_library(all_cooperate SHARED src/all_cooperate.cpp)

target_include_directories(all_cooperate
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

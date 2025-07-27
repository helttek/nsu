cmake_minimum_required(VERSION 3.27)

project(soft-tit-for-tat-strategy)

add_library(soft-tit-for-tat SHARED src/soft-tit-for-tat.cpp)

target_include_directories(soft-tit-for-tat
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

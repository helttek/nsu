cmake_minimum_required(VERSION 3.27)

project(tough-tit-for-tat-strategy)

add_library(tough-tit-for-tat SHARED src/tough-tit-for-tat.cpp)

target_include_directories(tough-tit-for-tat
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

cmake_minimum_required(VERSION 3.27)
project(stratery_api)

add_library(strategy_api STATIC
    src/strategy_factory.cpp
    src/strategy.cpp 
)

target_include_directories(strategy_api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
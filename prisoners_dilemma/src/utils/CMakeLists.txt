cmake_minimum_required(VERSION 3.27)

project(utils)

add_library(utils STATIC)

target_sources(utils PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exceptions.cpp
)

target_include_directories(utils PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
cmake_minimum_required(VERSION 3.28)
project(sound_processor)

set(CMAKE_CXX_STANDARD 17)

add_executable(sound_processor main.cpp
        Help.cpp
        Help.h)

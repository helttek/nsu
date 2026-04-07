cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_COMPILER gcc)

project(file_action_project)

add_executable(file_action)

target_compile_options(file_action PUBLIC -Wall)

target_sources(file_action PRIVATE 
    src/main.c
    src/util.c
    src/actions.c
)

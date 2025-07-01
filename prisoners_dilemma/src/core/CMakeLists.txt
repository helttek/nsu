cmake_minimum_required(VERSION 3.27)

project(core)

add_library(core STATIC 
    src/matrix.cpp
    src/fast_game.cpp
    src/detailed_game.cpp
    src/tournament.cpp
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include) 

target_link_libraries(core PRIVATE utils)

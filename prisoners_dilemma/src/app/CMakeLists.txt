cmake_minimum_required(VERSION 3.27)

add_executable(prisoners_dilemma)

target_sources(prisoners_dilemma PRIVATE main.cpp)

target_link_libraries(prisoners_dilemma PRIVATE core utils strategy_api)
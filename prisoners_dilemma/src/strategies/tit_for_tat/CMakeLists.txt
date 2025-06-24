cmake_minimum_required(VERSION 3.27)

project(tit_for_tat_strategy)

add_library(tit_for_tat SHARED src/tit_for_tat_strat.cpp)

set_target_properties(tit_for_tat PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/strategies # Output to build/strategies
    # RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/strategies # On Windows, DLLs are also RUNTIME
)
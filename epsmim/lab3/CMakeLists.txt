cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(lab3)

set(PROJECT_NAME lab3_3_rows )




set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "/bin/clang++")
set(SRC src/3_rows.cpp)
set(OPT -march=native -Ofast -g)
add_executable(lab3_3_rows ${SRC})
target_compile_options(lab3_3_rows PRIVATE ${OPT})

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND objdump -d $<TARGET_FILE:${PROJECT_NAME}> > ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.dump
        COMMENT "Generating objdump output for ${PROJECT_NAME}"
        VERBATIM
)

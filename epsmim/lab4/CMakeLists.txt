cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(lab3)

set(PROJECT_NAME first)

set(CMAKE_CXX_COMPILER clang++)

find_package(OpenMP REQUIRED)
set(CMAKE_CXX_STANDARD 20)
set(SRC1 src/first.cpp)
set(SRC2 src/second.cpp)
set(SRC3 src/third.cpp)
set(OPT1 -march=native )
set(OPT2 -march=native -Ofast -pthread -mavx2 -mfma)
set(OPT3 -march=native -Ofast)
add_executable(first ${SRC1})
add_executable(second ${SRC2})
add_executable(third ${SRC3})
target_compile_options(first PRIVATE ${OPT1})
target_compile_options(second PRIVATE ${OPT2})
target_compile_options(third PRIVATE ${OPT3})
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g -fopenmp")
target_link_libraries(first PRIVATE OpenMP::OpenMP_CXX)


add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND objdump -d $<TARGET_FILE:${PROJECT_NAME}> > ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.dump
        COMMENT "Generating objdump output for ${PROJECT_NAME}"
        VERBATIM
)


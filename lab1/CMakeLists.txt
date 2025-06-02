cmake_minimum_required(VERSION 3.28)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the project
project(lab1)

# Add compiler optimization flags
add_compile_options(-Ofast -g -pg)

# Add the executable target
add_executable(lab1 lab1.cpp)

# Generate assembly output
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/lab1.s
    COMMAND ${CMAKE_CXX_COMPILER} -S -masm=intel -Ofast -g -pg ${CMAKE_SOURCE_DIR}/lab1.cpp -o ${CMAKE_BINARY_DIR}/lab1.s
    DEPENDS lab1.cpp
    COMMENT "Generating assembly for lab1.cpp"
)

# Add a custom target so we can call `make asm`
add_custom_target(asm_output DEPENDS ${CMAKE_BINARY_DIR}/lab1.s)


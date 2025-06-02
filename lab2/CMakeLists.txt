cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

# Define the project name and enable languages (C, CXX)
project(lab2 LANGUAGES C CXX)

# Find packages needed for the project *after* project() is called.
find_package(OpenMP REQUIRED)

# Define source files and common options
set(SOURCES src/lab2.cpp)
set(SOURCES_INTRINSICS src/lab2_intrinsics.cpp)
set(SOURCES_INTRINSICS_ALIGNED src/lab2_intrinsics_aligned.cpp)
set(SOURCES_OPENMP_DIR src/lab2_openmp_dir.cpp)
set(STANDART_OPT -Ofast)

# Macro to select compiler and set appropriate flags
macro(select_compiler compiler_name)
  message(STATUS "Macro select_compiler called with: ${compiler_name}") # Debug message

  if(${compiler_name} STREQUAL "gcc")
    # Set the compiler *without* CACHE FORCE. Let CMake's detection work, guided by this if needed.
    # If problems persist, use -D CMAKE_CXX_COMPILER=g++ command line option instead.
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
    message(STATUS "Configuring for GCC...")

    # GCC specific flags
    set(OPENMP_OPT -fopenmp -fopenmp-simd -march=native)
    set(OPENMP_DIR_OPT -fopenmp)
    set(PARALLELIZATION -ftree-loop-distribution -floop-parallelize-all -floop-nest-optimize -floop-interchange -floop-strip-mine -fpeel-loops -fpredictive-commoning -march=native)
    set(INTRINSICS_OPT -march=native -Ofast)
    set(INTRINSICS_ALIGNED_OPT -march=native -Ofast)

    message(STATUS ">>> Defining GCC targets. OPENMP_OPT=${OPENMP_OPT}") # Debug

    # Add executables for GCC
    add_executable(lab2_gcc_no_opt ${SOURCES})
    add_executable(lab2_gcc_stdopt ${SOURCES})
    add_executable(lab2_gcc_openmp ${SOURCES})
    add_executable(lab2_gcc_parallelize_loops ${SOURCES})
    add_executable(lab2_gcc_intrinsics ${SOURCES_INTRINSICS})
    add_executable(lab2_gcc_intrinsics_aligned ${SOURCES_INTRINSICS_ALIGNED})
    add_executable(lab2_gcc_openmp_dir ${SOURCES_OPENMP_DIR})

    # Apply compile options for GCC targets
    target_compile_options(lab2_gcc_stdopt PRIVATE ${STANDART_OPT})
    target_compile_options(lab2_gcc_openmp PRIVATE ${OPENMP_OPT})
    target_compile_options(lab2_gcc_parallelize_loops PRIVATE ${PARALLELIZATION})
    target_compile_options(lab2_gcc_openmp_dir PRIVATE ${OPENMP_DIR_OPT})
    target_compile_options(lab2_gcc_intrinsics PRIVATE ${INTRINSICS_OPT})
    target_compile_options(lab2_gcc_intrinsics_aligned PRIVATE ${INTRINSICS_ALIGNED_OPT})

    # Link options/libraries for GCC targets
    target_link_options(lab2_gcc_openmp_dir PRIVATE -fopenmp) # Link OpenMP for the dir target
    target_link_libraries(lab2_gcc_openmp PRIVATE OpenMP::OpenMP_CXX) # Modern way


  elseif(${compiler_name} STREQUAL "clang")
    # Set the compiler *without* CACHE FORCE.
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++")
    message(STATUS "Configuring for Clang...")

    # Clang specific flags
    set(OPENMP_OPT -fopenmp -Rpass=openmp-opt -Rpass-missed=openmp-opt -Rpass-analysis=openmp-opt -march=native)
    set(OPENMP_DIR_OPT -fopenmp)
    set(PARALLELIZATION -fslp-vectorize -ftree-slp-vectorize -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -march=native)
    set(INTRINSICS_OPT -march=native -Ofast)
    set(INTRINSICS_ALIGNED_OPT -march=native -Ofast)

    message(STATUS ">>> Defining Clang targets. OPENMP_OPT=${OPENMP_OPT}") # Debug

    # Add executables for Clang
    add_executable(lab2_clang_no_opt ${SOURCES})
    add_executable(lab2_clang_stdopt ${SOURCES})
    add_executable(lab2_clang_openmp ${SOURCES})
    add_executable(lab2_clang_parallelize_loops ${SOURCES})
    add_executable(lab2_clang_intrinsics ${SOURCES_INTRINSICS})
    add_executable(lab2_clang_openmp_dir ${SOURCES_OPENMP_DIR})
    add_executable(lab2_clang_intrinsics_aligned ${SOURCES_INTRINSICS_ALIGNED})

    # Apply compile options for Clang targets
    target_compile_options(lab2_clang_parallelize_loops PRIVATE ${PARALLELIZATION})
    target_compile_options(lab2_clang_openmp_dir PRIVATE ${OPENMP_DIR_OPT})
    target_compile_options(lab2_clang_intrinsics PRIVATE ${INTRINSICS_OPT})
    target_compile_options(lab2_clang_intrinsics_aligned PRIVATE ${INTRINSICS_ALIGNED_OPT})

    set(PROJECT_NAME lab2_clang_intrinsics_aligned )
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND objdump -d $<TARGET_FILE:${PROJECT_NAME}> > ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.dump
        COMMENT "Generating objdump output for ${PROJECT_NAME}"
        VERBATIM
    )
    # Link options/libraries for Clang targets
    target_link_libraries(lab2_clang_openmp PRIVATE OpenMP::OpenMP_CXX)
    target_link_libraries(lab2_clang_openmp_dir PRIVATE OpenMP::OpenMP_CXX)

  else()
    message(WARNING "Unknown compiler selected in macro: ${compiler_name}. Build targets might be incorrect.")
  endif()
endmacro()

# Select the desired compiler *after* project() and macro definition
# This *should* ensure only the GCC block of the macro is executed in the final configuration.
select_compiler("clang")

# --- Custom Commands/Targets ---
set(ASSEMBLY_OUTPUT_FILE_GCC "${CMAKE_BINARY_DIR}/lab2_gcc_intrinsics_aligned.s")
# Ensure the INTRINSICS_ALIGNED_OPT variable is set correctly when this runs.
# If the macro logic works, it should be the GCC version.
add_custom_command(
    OUTPUT ${ASSEMBLY_OUTPUT_FILE_GCC}
    COMMAND ${CMAKE_CXX_COMPILER} ${INTRINSICS_ALIGNED_OPT} -S ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCES_INTRINSICS_ALIGNED} -o ${ASSEMBLY_OUTPUT_FILE_GCC}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCES_INTRINSICS_ALIGNED}
    COMMENT "Generating assembly code (GCC) for ${SOURCES_INTRINSICS_ALIGNED}"
    VERBATIM
)
add_custom_target(assembly_gcc DEPENDS ${ASSEMBLY_OUTPUT_FILE_GCC})

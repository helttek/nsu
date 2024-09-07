.PHONY: clean all
COMPILER=g++
FLAGS=-std=c++20 -Wall -Wextra -O2
SOURCES=main.cpp CubicEquationSolver.cpp QuadraticEquation.cpp SegmentSolution.cpp
EXECUTABLE=calc_cubic_equation

all: clean $(EXECUTABLE)

$(EXECUTABLE):
	$(COMPILER) $(FLAGS) $(SOURCES) -o $@

clean:
	rm -rf $(EXECUTABLE)

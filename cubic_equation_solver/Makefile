.PHONY: clean all
COMPILER=g++
FLAGS=-Wall -Wextra -std=c++20
SOURCES=main.cpp CubicEquationSolver.cpp QuadraticEquation.cpp
EXECUTABLE=calc_cubic_equation

all: clean $(EXECUTABLE)

$(EXECUTABLE):
	$(COMPILER) $(FLAGS) $(SOURCES) -o $@

clean:
	rm -rf $(EXECUTABLE)

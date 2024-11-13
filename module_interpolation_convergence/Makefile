.PHONY: clean all
COMPILER=g++
FLAGS=-std=c++20 -Wall -Wextra -O2
SOURCES=main.cpp NewtonPolynomial.cpp
EXECUTABLE=module_interpolation_convergence

all: clean $(EXECUTABLE)

$(EXECUTABLE):
	$(COMPILER) $(FLAGS) $(SOURCES) -o $@

clean:
	rm -rf $(EXECUTABLE)

COMPILER = /usr/bin/mpicxx
OPTIONS = -O2 -I_MPI_HYDRA_BRANCH_COUNT -I_MPI_LSF_USE_COLLECTIVE_LAUNCH

duplicated: duplicated.cpp
	$(COMPILER) duplicated.cpp -o duplicated $(OPTIONS)

scattered: scattered.cpp
	$(COMPILER) scattered.cpp -o scattered $(OPTIONS)
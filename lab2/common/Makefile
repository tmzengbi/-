
n:=4

matrix.so: matrix.h matrix.cpp
	@g++ matrix.cpp -shared -fPIC -o libmatrix.so

.PHONY: libtest
libtest:
	@export LD_LIBRARY_PATH=.
	@g++ main.cpp -L . -lmatrix -o main
	@./main

.PHONY: test
test:
	@cat /dev/null > a.out
	@mpicc test.c -o test
	@mpirun -n $n ./test

.PHONY: mpi_test

.PHONY: generate
generate:
	python3 generate.py $n
mpi_test:
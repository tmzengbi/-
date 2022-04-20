.PHONY: subtest1
subtest1:
	g++ matrix.cpp -fopenmp -o test -DDEBUG
	./test 4

.PHONY: subtest2
subtest2:
	g++ matrix_schedule_dynamic.cpp -fopenmp -o test -DDEBUG
	./test 4
	g++ matrix_schedule_static.cpp -fopenmp -o test -DDEBUG
	./test 4

.PHONY: subtest3
subtest3:
	gcc parallel_for.c -FPIC -shared -o libparallel_for.so -lpthread

.PHONY: subtest3_DEBUG
subtest3_DEBUG:
	export LD_LIBRARY_PATH=. && \
	gcc test3.c -L. -lparallel_for -o test && \
	./test 4


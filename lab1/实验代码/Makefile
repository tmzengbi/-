GXX = g++
V ?= -O2
GXX_FLAG = -Wall -std=c++17 $(V)
LINK = blas64
TARGET = matrix
matrix: matrix.o
	$(GXX) $^ $(GXX_FLAG) -l$(LINK) -o $(TARGET) 

matrix.o: matrix.cpp matrix.hpp util.hpp
	$(GXX) $(GXX_FLAG) -c matrix.cpp

clean:
	@rm -rf *.o *.gch
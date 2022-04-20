#! /bin/bash
#
mpicc -c -Wall -fopenmp heated_plate_mpi.c
if [ $? -ne 0 ]; then
  echo "Compile error."
  exit
fi
#
mpicc -fopenmp heated_plate_mpi.o -lm
if [ $? -ne 0 ]; then
  echo "Load error."
  exit
fi
rm heated_plate_mpi.o
mv a.out $HOME/binc/heated_plate_mpi
#
echo "Normal end of execution."

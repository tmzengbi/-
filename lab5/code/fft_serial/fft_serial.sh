#! /bin/bash
#
g++ -c -Wall fft_serial.cpp -I../
if [ $? -ne 0 ]; then
  echo "Compile error."
  exit
fi
#
export LD_LIBRARY_PATH=. && \
g++ fft_serial.o -lm -L.. -lparallel_for
if [ $? -ne 0 ]; then
  echo "Load error."
  exit
fi
#
rm fft_serial.o
#
chmod ugo+x a.out
mv a.out ~/binc/fft_serial
#
echo "Normal end of execution."

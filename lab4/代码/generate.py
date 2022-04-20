# 随机矩阵生成器
# argc == 1
# python3 generate.py args[1]
# args[1] 表示生成两个 args[1] * args[1] 的随机矩阵 （随机数范围 [0,10]）
import random
import sys

if len(sys.argv) != 2:
    print('matrix size N (N X N) needed')

f = open('a.in', "w")

N = int(sys.argv[1])
f.write('%d %d %d\n'%(N, N, N))
for i in range(N):
    for j in range(N):
        f.write('%d '%random.randint(1,10))
    f.write('\n')


f.write('\n')
for i in range(N):
    for j in range(N):
        f.write('%d '%random.randint(1,10))
    f.write('\n')
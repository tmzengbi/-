import sys
import random
argv = sys.argv

if len(argv) != 2:
    print('error')
    exit(-1)
n = int(argv[1])
f = open('a.in', 'w')

for i in range(n):
    for j in range(n):
        data = random.randint(1,100)
        f.write(str(data) + ' ')
    f.write('\n')



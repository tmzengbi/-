try:
    while True:
        input()
        for i in [256, 512, 1024, 2048]:
            input()
            print('%d X %d matrix'%(i,i))
            for p in range(1, 9):
                input()
                # print('use %d thread'%p)
                sum = 0
                for _ in range(5):
                    data = float(input())
                    sum += data
                print('%.10f'%(sum / 5))
except EOFError:
    print('end file reading')
except BaseException:
    print('Unexpected error')

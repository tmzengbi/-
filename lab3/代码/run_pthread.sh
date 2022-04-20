# pthread 多线程运行脚本
for file in matrix_block.cpp matrix_loop.cpp; do
    echo "start solve $file"
    for size in 256 512 1024 2048; do
        echo "generate $size X $size matrix"
        python3 generate.py $size # generate ramdom data
        g++ $file -o test -lpthread
        for ((th=1; th<=10; th++)); do
            echo "use $th threads"
            for ((i=1; i<=5; i++)) ;do 
                ./test $th
                sleep 0.1
            done        
        done
    done
done
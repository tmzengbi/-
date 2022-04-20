# 运行脚本
for file in $@; do
    echo "start solve $file"
    g++ $file -o test -fopenmp
    for size in 4096; do #256 512 1024 2048; do
        echo "generate $size X $size matrix"
        python3 generate.py $size # generate ramdom data
        for ((p=1; p<=8; p++)); do
            echo "use $p thread "
            for ((i=1; i<=5; i++)) ;do 
                # mpiexec -n $p ./test
                ./test $p
                sleep 0.1
            done        
        done
    done
done
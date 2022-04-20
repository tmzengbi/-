# mpi 运行脚本
for file in mpi_p2p.c mpi_gather.c mpi_createStruct.c; do
    echo "start solve $file"
    mpicc $file -o test
    for size in 256 512 1024 2048; do
        echo "generate $size X $size matrix"
        python3 generate.py $size # generate ramdom data
        for ((p=1; p<=10; p++)); do
            echo "use $p process "
            for ((i=1; i<=5; i++)) ;do 
                mpiexec -n $p ./test
                sleep 0.1
            done        
        done
        p=16
        echo "use $p process "
        for ((i=1; i<=5; i++)) ;do 
            mpiexec -n $p ./test
            sleep 0.1
        done    
    done
done
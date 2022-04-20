#include "parallel_for.h"
#include <pthread.h>

void parallel_for(int start, int end, int increment, void *(*function)(void *), void *arg, int num_threads) {
    // 1. get the number of work
    // 首先将 end 对齐，end = \lceil \frac{end}{increament} \rceil * increment
    end = (end + increment - 1) / increment * increment;
    // 然后计算 task 数量
    int tasks = (end - start) / increment;
    // 2. partition tasks
    // 循环分配，因此 increment 变为 increment * num_threads
    pthread_t worker[num_threads];
    struct for_index idx[num_threads];
    for(int i = 0; i < num_threads; ++ i) {
        idx[i].thread_id = i;
        idx[i].thread_num = num_threads;
        idx[i].arg = arg;
        idx[i].start = start + increment * i;
        idx[i].end = end;
        idx[i].increment = increment * num_threads;
        if(i > 0) {
            pthread_create(&worker[i], NULL, function, idx + i);
        }
    }
    function(idx);
    for(int i = 1; i < num_threads; ++ i)
        pthread_join(worker[i], NULL);
}

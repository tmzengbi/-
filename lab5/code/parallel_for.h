#ifndef PARALLEL_FOR_H

#define PARALLEL_FOR_H

#ifdef __cplusplus
extern "C"  {
#endif
struct for_index {
    void *arg;
    int start;
    int end;
    int increment;
    int thread_id;
    int thread_num;
};
void parallel_for(int start, int end, int increment, void *(*function)(void *), void *arg, int num_threads);
#ifdef __cplusplus
}
#endif

#endif
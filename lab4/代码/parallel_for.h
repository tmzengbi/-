struct for_index {
    void *arg;
    int start;
    int end;
    int increment;
    int thread_id;
    int thread_num;
};
void parallel_for(int start, int end, int increment, void *(*function)(void *), void *arg, int num_threads);

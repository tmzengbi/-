/*
多线程数组求和（每次取 1 个元素）
*/
#include <pthread.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <chrono>
using namespace std;
pthread_mutex_t mutex;
int siz;
int global_index = 0;
#define MAXN 10000000
int a[MAXN];
void *solve(void *argv) {
    int *sum = (int*) argv;
    while(true) {
        pthread_mutex_lock(&mutex);
        if(global_index == MAXN) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        int id = global_index++;
        pthread_mutex_unlock(&mutex);
        *sum += a[id];
    }
    return NULL;
}
void init() {
    for(int i = 0; i < MAXN; ++i)
        a[i] = 1;
}
int main(int argc, char *argv[]) {
    if(argc != 2) {
        cerr << "number of thread needed\n" ;
        exit(-1);
    }
    siz = boost::lexical_cast<int> (argv[1]);
    init();
    pthread_t thread[siz];
    int sum[siz];
    memset(sum, 0 ,sizeof sum);
    pthread_mutex_init(&mutex, NULL);
    auto Start = chrono::steady_clock().now();
    for(int i = 0; i < siz; ++ i)
        pthread_create(thread + i, NULL, solve, sum + i);
    for(int i = 0; i < siz; ++ i)
        pthread_join(thread[i], NULL);
    // for(int i = 0; i < siz; ++ i) {
    //     cout<<"thread"<<i<<" get sum "<<sum[i]<<endl;
    // }
    auto End = chrono::steady_clock().now();
    cout << chrono::duration<double>(End - Start).count() << endl;
}
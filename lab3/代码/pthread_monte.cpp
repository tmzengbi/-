/*
多线程 Monte-carlo 方法求积分
*/
#include <iostream>
#include <pthread.h>
#include <boost/lexical_cast.hpp>
#include <random>
using namespace std;
#define MAXN 1000000
#define MAXNUM 10000
int siz;
int tot = 0;
pthread_mutex_t mutex;

void* fun(void *argv) {
    unsigned int seed = *(unsigned int*) argv;
    mt19937 rnd(seed);
    int sum = 0;
    for(int i = 0; i < MAXN; ++ i) {
        int x = rnd() % (MAXNUM + 1); // num is [0, MAX]
        int y = rnd() % (MAXNUM + 1);
        // (y/M) <= (x/M)^2 means My <= x^2
        // printf("%d %d %d %d\n", x, y, MAXNUM * y, x * x);
        if(MAXNUM * y <= x * x) ++ sum;
    }
    tot += sum;
    printf("thread get answer %d/%d=%.10lf\n", sum, MAXN, 1.0 * sum / MAXN);
    return NULL;
}
int main(int argc, char *argv[]) {
    if(argc != 2) {
        cerr << "number of thread needed\n" ;
        exit(-1);
    }
    siz = boost::lexical_cast<int> (argv[1]);
    mt19937 rnd(time(NULL));
    pthread_mutex_init(&mutex, NULL);
    pthread_t thread[siz];
    unsigned int seed[siz];
    for(int i = 0; i < siz; ++ i) {
        seed[i] = rnd();
        pthread_create(thread + i, NULL, fun, seed + i);
    }
    for(int i = 0; i < siz; ++ i)
        pthread_join(thread[i], NULL);
    printf("total get answer %d/%d=%.10lf\n", tot, MAXN * siz, 1.0 * tot / (MAXN * siz));
}
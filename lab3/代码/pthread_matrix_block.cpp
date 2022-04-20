/*
块划分实现多线程矩阵乘法
*/
#include <pthread.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <chrono>

using namespace std;
int siz;
int N, M, K;
float *A, *B, *C; // A is N * M .. B is M * K .. C is N * K
void *solve(void *argv) { 
    int id = *(int*) argv;
    #define idA (i * M + k)
    #define idB (k * K + j)
    #define idC (i * K + j)
    int block = (N + siz - 1) / siz; 
    for(int i = id; i < id + block; ++ i) {
        if(i >= N) break;
        for(int k = 0; k < M; ++ k) {
            float x = A[idA];
            for(int j = 0; j < K; ++ j) {
                C[idC] += x * B[idB];
            }
        }
    }
    return NULL;
}
int main(int argc, char *argv[]) {
    if(argc != 2) {
        cerr << "number of thread needed\n" ;
        exit(-1);
    }
    siz = boost::lexical_cast<int> (argv[1]);
    fstream in("a.in");
    in>>N>>M>>K;
    A = (float*) malloc(sizeof(float) * N * M);
    B = (float*) malloc(sizeof(float) * M * K);
    C = (float*) malloc(sizeof(float) * N * K);
    for(int i = 0; i < N * M; ++ i) in>>A[i];
    for(int i = 0; i < M * K; ++ i) in>>B[i];
    auto Start = chrono::steady_clock().now();
    pthread_t thread[siz];
    int id[siz];
    for(int i = 0; i < siz; ++ i) {
        id[i] = i;
        pthread_create(&thread[i], NULL, solve, (void*)(id + i)); // send the tid
    }
    for(int i = 0; i < siz; ++ i)
        pthread_join(thread[i], NULL);
    #ifdef DEBUG
    for(int i = 0; i < N * K; ++ i) {
        printf("%f ",C[i]);
        if(i % K == K - 1) puts("");
    }
    #endif
    auto End = chrono::steady_clock().now();
    cout << chrono::duration<double>(End - Start).count() << endl;

}
/*
4 4 4
1 2 3 4
1 2 3 4
1 2 3 4
1 2 3 4

1 2 3 4
1 2 3 4
1 2 3 4
1 2 3 4

*/
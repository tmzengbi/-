#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <chrono>

int N,M,K;
float *A, *B, *C;
// A is N * M
// B is M * K
// C is N * K
void solve() {
    #define idA (i * M + k) 
    #define idB (k * K + j)
    #define idC (i * K + j)
    int size = omp_get_num_threads();
    int rank = omp_get_thread_num();
    for(int i = rank; i < N; i += size)
        for(int k = 0; k < M; ++ k) {
            float tmp = A[idA];
            for(int j = 0; j < K; ++ j)
                C[idC] += tmp * B[idB];
        }
    #undef idA
    #undef idB
    #undef idC
}
int main(int argc, char *argv[]) {
    if(argc != 2) {
        puts("number of thread needed");
        exit(-1);
    }
    int size = strtol(argv[1], NULL, 10);
    FILE *fp;
    if((fp = fopen("a.in", "r")) == NULL) {
        perror("cannot open file");
        exit(-1);
    }
    fscanf(fp, "%d%d%d", &N, &M, &K);
    A = (float*) malloc (sizeof(float) * N * M);
    B = (float*) malloc (sizeof(float) * M * K);
    C = (float*) malloc (sizeof(float) * N * K);
    memset(C, 0, sizeof(float) * N * K);
    for(int i = 0; i < N * M; ++ i)
        fscanf(fp, "%f", A + i);
    for(int i = 0; i < M * K; ++ i)
        fscanf(fp, "%f", B + i);
    auto start = std::chrono::steady_clock::now();
#pragma omp parallel num_threads(size)
    solve();
    #ifdef DEBUG
    for(int i = 0; i < N * K; ++ i) {
        printf("%f ",C[i]);
        if(i % K == K - 1) {
            puts("");
        }
    }
    #endif
    auto duration = std::chrono::steady_clock::now() - start;
    double lasting = std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
    printf("%.10lf\n", lasting);
}
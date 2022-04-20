#include "parallel_for.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int N,M,K;
float *A, *B, *C;
void *solve(void *arg) {
    struct for_index *idx = (struct for_index*) arg;
    int start = idx->start;
    int end = idx->end;
    int increment = idx->increment;
    for(int i = start; i < end; i += increment) {
        #define idA (i * M + k) 
        #define idB (k * K + j)
        #define idC (i * K + j)
        for(int k = 0; k < M; ++ k) {
            float tmp = A[idA];
            for(int j = 0; j < K; ++ j)
                C[idC] += tmp * B[idB];
        }
        #undef idA
        #undef idB
        #undef idC
    }
    return NULL;
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
    // end reading data from file
    parallel_for(0, N, 1, solve, NULL, size);
    // end parallel_for
    for(int i = 0; i < N * K; ++ i) {
        printf("%f ",C[i]);
        if(i % K == K - 1) {
            puts("");
        }
    }
}
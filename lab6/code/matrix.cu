#ifndef __CUDACC__
#define __CUDACC__
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cuda_runtime.h>

using namespace std;

__global__ void solve(float *A, float *B, float *C, int N, int M, int K) {
    #define idA (i * M + k) 
    #define idB (k * K + j)
    #define idC (i * K + j)

    int threadId = blockIdx.x * blockDim.x + threadIdx.x;
    int i = threadId;
    if(i >= N) return;
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
        fprintf(stderr, "Number of block size needed");
        exit(-1);
    }
    const int THREADS_PER_BLOCK = strtol(argv[1], NULL, 10);
    float *A, *B, *C;
    int N, M, K;
    FILE *fp;
    if((fp = fopen("a.in", "r")) == NULL) {
        perror("cannot open file");
        exit(-1);
    }
    fread(&N, 1, sizeof(int), fp);
    fread(&M, 1, sizeof(int), fp);
    fread(&K, 1, sizeof(int), fp);
    A = (float*) malloc (sizeof(float) * N * M);
    B = (float*) malloc (sizeof(float) * M * K);
    C = (float*) malloc (sizeof(float) * N * K);
    fread(A, sizeof(float), N * M, fp);
    fread(B, sizeof(float), M * K, fp);
    #ifdef DEBUG
    printf("Matrix A:\n");
    for(int i = 0; i < N * M; ++ i) {
        printf("%f ",A[i]);
        if(i % M == M - 1)
            printf("\n");
    }
    printf("Matrix B:\n");
    for(int i = 0; i < M * K; ++ i) {
        printf("%f ",B[i]);
        if(i % K == K - 1)
            printf("\n");
    }
    #endif
    float *d_A, *d_B, *d_C;
    cudaMalloc((void**) &d_A, sizeof(float) * N * M);
    cudaMalloc((void**) &d_B, sizeof(float) * M * K);
    cudaMalloc((void**) &d_C, sizeof(float) * N * K);
    printf("start copying data from Host to Device\n");
    cudaMemcpy(d_A, A, sizeof(float) * N * M, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B, sizeof(float) * M * K, cudaMemcpyHostToDevice);
    cudaMemset(d_C, 0, sizeof(float) * N * K);
    printf("start calculating\n");
    cudaEvent_t e_start, e_stop;
    cudaEventCreate(&e_start);
    cudaEventCreate(&e_stop);

    cudaEventRecord(e_start, 0);
    solve<<<(N + THREADS_PER_BLOCK -1) / THREADS_PER_BLOCK ,THREADS_PER_BLOCK>>>(d_A, d_B, d_C, N, M, K);
    cudaEventRecord(e_stop, 0);

    cudaEventSynchronize(e_stop);
    float elapsedtime;
    cudaEventElapsedTime(&elapsedtime, e_start, e_stop);
    printf("finish calculating, costing %f\n", elapsedtime);

    printf("start copying data from Device to Host\n");
    cudaMemcpy(C, d_C, sizeof(float) * N * K,cudaMemcpyDeviceToHost);
    printf("Normal execution\n");
    #ifdef DEBUG
    for(int i = 0; i < N * K; ++ i) {
        printf("%f ",C[i]);
        if(i % K == K - 1)
            printf("\n");
    }
    #endif
    cudaError_t err = cudaGetLastError();
    printf("%s\n",cudaGetErrorString(err));
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
    free(A);
    free(B);
    free(C);
}

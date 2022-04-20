#ifndef __CUDACC__
#define __CUDACC__
#endif

#include <cstdio>
#include <cstring>
#include <cuda_runtime.h>
#include <cublas_v2.h>
using namespace std;

int main(int argc, char *argv[]) {
    float *A, *B, *C;
    const float alpha = 1;
    const float beta = 0;
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
    
    printf("start calculating\n");
    cudaEvent_t e_start, e_stop;
    cudaEventCreate(&e_start);
    cudaEventCreate(&e_stop);

    cudaEventRecord(e_start, 0);
    cublasHandle_t handle;
    cublasCreate(&handle);
    cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, K, M, N, &alpha, d_B, K, d_A, N, &beta, d_C, K);
    cublasDestroy(handle);
    cudaEventRecord(e_stop, 0);

    cudaEventSynchronize(e_stop);
    float elapsedtime;
    cudaEventElapsedTime(&elapsedtime, e_start, e_stop);
    printf("finish calculating, costing %f\n", elapsedtime);
    
    printf("start copying data from Device to Host\n");
    cudaMemcpy(C, d_C, sizeof(float) * N * K, cudaMemcpyDeviceToHost);
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

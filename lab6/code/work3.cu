#ifndef __CUDACC__
#define __CUDACC__
#endif

#include <cstdio>
#include <cstring>
#include <cuda_runtime.h>
#include <cublas_v2.h>
using namespace std;

#define stride 1
#define THREADS_PER_BLOCK 32


__global__ void cudacnn(float *input, float *kernel, float *out, int N, int M, int K) {
    int threadId_x = blockIdx.x * blockDim.x + threadIdx.x;
    int threadId_y = blockIdx.y * blockDim.y + threadIdx.y;
    if(threadId_x >= K || threadId_y >= K)
        return;
    // printf("%d %d\n", threadId_x, threadId_y);
    int x = threadId_x * stride, y = threadId_y * stride;
    float sum = 0;
    for(int i = 0; i < M ; ++ i) 
        for(int j = 0; j < M ; ++ j) {
            if(x + i < N && y + j < N) 
                sum += input[(x + i) * N + (y + j)] * kernel[i * M + j];
        }
    out[threadId_x * K + threadId_y] = sum;
}   

int main(int argc, char *argv[]) {
    float *input, *kernel, *output;
    int N, M, K;
    FILE *fp;
    if((fp = fopen("a.in", "r")) == NULL) {
        perror("cannot open file");
        exit(-1);
    }
    fread(&N, 1, sizeof(int), fp);
    fread(&M, 1, sizeof(int), fp);
    // K = (N - M + 1 + stride - 1) / stride
    K = (N - M + stride) / stride;
    input = (float*) malloc (sizeof(float) * N * N);
    kernel = (float*) malloc (sizeof(float) * M * M);
    output = (float*) malloc (sizeof(float) * K * K);
    fread(input, sizeof(float), N * N, fp);
    fread(kernel, sizeof(float), M * M, fp);
    #ifdef DEBUG
    printf("Matrix input:\n");
    for(int i = 0; i < N * N; ++ i) {
        printf("%f ",input[i]);
        if(i % N == N - 1)
            printf("\n");
    }
    printf("Matrix kernel:\n");
    for(int i = 0; i < M * M; ++ i) {
        printf("%f ",kernel[i]);
        if(i % M == M - 1)
            printf("\n");
    }
    #endif
    float *d_input, *d_kernel, *d_output;
    cudaMalloc((void**) &d_input, sizeof(float) * N * N);
    cudaMalloc((void**) &d_kernel, sizeof(float) * M * M);
    cudaMalloc((void**) &d_output, sizeof(float) * K * K);

    printf("start copying data from Host to Device\n");

    cudaMemcpy(d_input, input, sizeof(float) * N * N, cudaMemcpyHostToDevice);
    cudaMemcpy(d_kernel, kernel, sizeof(float) * M * M, cudaMemcpyHostToDevice);
    
    printf("start calculating\n");
    cudaEvent_t e_start, e_stop;
    cudaEventCreate(&e_start);
    cudaEventCreate(&e_stop);

    cudaEventRecord(e_start, 0);
    
    dim3 block((K + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK, (K + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK);
    dim3 threads(THREADS_PER_BLOCK, THREADS_PER_BLOCK);
    cudacnn<<<block, threads>>>(d_input, d_kernel, d_output, N, M, K);

    cudaEventRecord(e_stop, 0);

    cudaEventSynchronize(e_stop);
    float elapsedtime;
    cudaEventElapsedTime(&elapsedtime, e_start, e_stop);
    printf("finish calculating, costing %f\n", elapsedtime);
    
    printf("start copying data from Device to Host\n");
    cudaMemcpy(output, d_output, sizeof(float) * K * K, cudaMemcpyDeviceToHost);
    printf("Normal execution\n");
    #ifdef DEBUG
    for(int i = 0; i < K * K; ++ i) {
        printf("%f ",output[i]);
        if(i % K == K - 1)
            printf("\n");
    }
    #endif
    cudaError_t err = cudaGetLastError();
    printf("%s\n",cudaGetErrorString(err));
    cudaFree(d_input);
    cudaFree(d_output);
    cudaFree(d_kernel);
}

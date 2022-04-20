/*
mpi 聚合通信矩阵乘法
*/
#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
// x is N * M , y is M * K and z is N * K
void solve(float *x, float *y, float *z, int N, int M, int K) {
    #define idx (i * M + k) 
    #define idy (k * K + j)
    #define idz (i * K + j)
    memset(z, 0, sizeof(float) * N * K);
    for(int i = 0; i < N; ++ i) {
        for(int k = 0; k < M; ++ k) {
            float tmp = x[idx];
            for(int j = 0; j < K; ++ j) {
                z[idz] += tmp * y[idy];
            }
        }
    }
    #undef idx
    #undef idy
    #undef idz
}
int siz, rank;
int N, M, K;
int main(int argc, char *argv[]) {
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &siz);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int n, m, k;
    float *A, *B, *C;
    if(rank == 0) {
        FILE* f = fopen("a.in", "r");
        fscanf(f, "%d%d%d", &N, &M, &K);
        n = (N + siz - 1) / siz;
        m = M;
        k = K;
        A = (float*) malloc(n * m * siz * sizeof(float));
        B = (float*) malloc(m * k * sizeof(float));
        C = (float*) malloc(n * k * siz * sizeof(float));
        memset(A, 0, n * m * siz * sizeof(float));
        for(int i = 0; i < N * M; ++ i)
            fscanf(f, "%f", A + i);
        for(int i = 0; i < M * K; ++ i)
            fscanf(f, "%f", B + i);
    }
    double start = MPI_Wtime();
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank != 0)
        B = (float*) malloc(m * k * sizeof(float));
    float *x, *res;
    x = (float*) malloc(n * m * sizeof(float));
    res = (float*) malloc(n * k * sizeof(float));
    MPI_Scatter(A, n * m, MPI_FLOAT, x, n * m, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, m * k, MPI_FLOAT, 0, MPI_COMM_WORLD);
    solve(x, B, res, n, m, k);
    MPI_Gather(res, n * k, MPI_FLOAT, C, n * k, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if(rank == 0) {
        // for(int i = 0; i < N * K; ++ i) {
        //     printf("%f " ,C[i]);
        //     if(i % K == K - 1) puts("");
        // }
        double end = MPI_Wtime();
        printf("%lf\n", end - start);
        free(A);
        free(C);
    }
    free(B);
    free(x);
    free(res);
    MPI_Finalize();
}
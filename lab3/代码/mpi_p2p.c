/*
mpi 点对点通信矩阵乘法
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
    if(rank == 0) {
        FILE* f = fopen("a.in", "r");
        fscanf(f, "%d%d%d", &N, &M, &K);
        int n = (N + siz - 1) / siz, m = M, k = K;
        int realN = n * siz;
        float *A, *B, *C;
        A = (float*) malloc(sizeof(float) * realN * m);
        B = (float*) malloc(sizeof(float) * m * k);
        C = (float*) malloc(sizeof(float) * realN * K); 
        memset(A, 0, sizeof(float) * realN * M);
        for(int i = 0; i < N * M; ++ i)
            fscanf(f, "%f", A + i);
        for(int i = 0; i < M * K; ++ i)
            fscanf(f, "%f", B + i);
        double start = MPI_Wtime();
        for(int i = 1; i < siz; ++ i) {
            MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&m, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&k, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
            MPI_Send(A + n * m * i, n * m, MPI_FLOAT, i, 3, MPI_COMM_WORLD);
            MPI_Send(B, m * k, MPI_FLOAT, i, 4, MPI_COMM_WORLD);
        }
        solve(A, B, C, n, m, k);
        for(int i = 1; i < siz; ++ i)
            MPI_Recv(C + i * n * k, n * k, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // for(int i = 0; i < N * K; ++ i) {
        //     printf("%f " ,C[i]);
        //     if(i % K == K - 1) puts("");
        // }
        double end = MPI_Wtime();
        printf("%lf\n", end - start);
        free(A);
        free(B);
        free(C);
    } else {
        int n,m,k;
        float *A, *B, *C;
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&m, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&k, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        A = (float*) malloc(sizeof(float) * n * m);
        B = (float*) malloc(sizeof(float) * m * k);
        C = (float*) malloc(sizeof(float) * n * k); 
        MPI_Recv(A, n * m, MPI_FLOAT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B, m * k, MPI_FLOAT, 0, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        solve(A, B, C, n, m, k);
        MPI_Send(C, n * k, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        free(A);
        free(B);
        free(C);
    }
    MPI_Finalize();
}
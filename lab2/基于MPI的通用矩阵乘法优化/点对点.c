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
const int n = 4;
int main() {
    srand(time(NULL));
    int size, rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    float *mat = (float*) calloc(n * n, sizeof(float));
    double begin = MPI_Wtime();
    if(rank == 0) {
        FILE *f = fopen("a.in", "r");
        if(f == NULL) {
            perror("error opening file");
            exit(-1);
        }
        for(int i = 0; i < n * n; ++ i)
            fscanf(f, "%f", mat + i);
        fclose(f);
        for(int i = 1; i < size; ++ i)
            MPI_Send(mat, n * n, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        for(int i = 1; i < size; ++ i) {
            int tot = n / (size - 1);
            if(i == size - 1) 
                tot = n - tot * (size - 2);
            MPI_Recv(mat + n / (size - 1) * (i - 1) * n, tot * n, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        #ifdef DEBUG
        for(int i = 0; i < n * n; ++ i) {
            printf("%f ",mat[i]);
            if(i % n == n - 1)
                puts("");
        }
        #endif
        double end = MPI_Wtime();
        printf("process%d cost %lf\n", rank, end - begin);
    } else {
        MPI_Recv(mat, n * n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int tot = n / (size - 1);
        if(rank == size -1) 
            tot = n - tot * (size - 2);
        float *res = (float*) calloc(tot * n, sizeof(float));
        solve(mat + n / (size - 1) * (rank - 1) * n, mat, res, tot, n, n);
        MPI_Send(res, tot * n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        free(res);
    }
    free(mat);
    MPI_Finalize();
}
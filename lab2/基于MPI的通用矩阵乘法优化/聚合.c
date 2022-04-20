#include <mpi/mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>

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
int main() {
    MPI_Init(NULL, NULL);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    const int n = 2048;
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
    }
    float *a = (float*) calloc(n / size * n, sizeof(float));
    float *res = (float*) calloc(n / size * n, sizeof(float));
    MPI_Bcast(mat, n * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Scatter(mat, n / size * n, MPI_FLOAT, a, n / size * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    solve(a, mat, res, n / size, n, n);
    // FILE *f = fopen("a.out", "a");
    // int fd = fileno(f);
    // flock(fd, LOCK_EX);
    // fprintf(f, "run in process %d\n", rank);
    // for(int i = 0; i < n / size; ++ i) {
    //     for(int j = 0; j < n; ++ j)
    //         fprintf(f, "%f ", res[i * n + j]);
    //     fprintf(f, "\n");
    // }
    // flock(fd, LOCK_UN);
    MPI_Gather(res, n * n / size, MPI_FLOAT, mat, n * n / size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if(rank == 0) {
        #ifdef DEBUG
        for(int i = 0; i < n; ++ i) {
            for(int j = 0; j < n; ++ j)
                printf("%f ",mat[i * n + j]);
            puts("");
        }
        #endif
        double end = MPI_Wtime();
        printf("process%d cost %lf\n", rank, (end - begin));
    }
    free(mat);
    free(res);
    MPI_Finalize();
}
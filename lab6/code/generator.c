#include <stdlib.h>
#include <time.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
    srand(time(NULL));
    if(argc != 2) {
        printf("matrix size N (N X N) needed\n");
        exit(-1);
    }
    int N = strtol(argv[1], NULL, 10);
    int M = N, K = N;
    FILE *fp;
    if((fp = fopen("a.in", "w")) == NULL) {
        perror("cannot open file");
        exit(-1);
    }
    fwrite(&N, sizeof(int), 1, fp);
    fwrite(&M, sizeof(int), 1, fp);
    fwrite(&K, sizeof(int), 1, fp);
    for(int i = 0; i < N * M; ++ i) {
        float x = rand() % 5 + 1;
        fwrite(&x, sizeof(float), 1, fp);
    }
    for(int i = 0; i < M * K; ++ i) {
        float x = rand() % 5 + 1;
        fwrite(&x, sizeof(float), 1, fp);
    }
}
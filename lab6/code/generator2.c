#include <stdlib.h>
#include <time.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
    srand(time(NULL));
    if(argc != 3) {
        printf("Input size N * N and Kernel size M * M needed\n");
        exit(-1);
    }
    int N = strtol(argv[1], NULL, 10);
    int M = strtol(argv[2], NULL, 10);
    FILE *fp;
    if((fp = fopen("a.in", "w")) == NULL) {
        perror("cannot open file");
        exit(-1);
    }
    fwrite(&N, sizeof(int), 1, fp);
    fwrite(&M, sizeof(int), 1, fp);
    for(int i = 0; i < N * N; ++ i) {
        float x = rand() % 5 + 1;
        fwrite(&x, sizeof(float), 1, fp);
    }
    for(int i = 0; i < M * M; ++ i) {
        float x = rand() % 5 + 1;
        fwrite(&x, sizeof(float), 1, fp);
    }
}
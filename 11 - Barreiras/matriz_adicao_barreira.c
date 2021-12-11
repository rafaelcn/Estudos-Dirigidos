#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXSIZE 10000  /* maximum matrix size */

pthread_barrier_t barrier;

int size;

int matrix1[MAXSIZE][MAXSIZE];
int matrix2[MAXSIZE][MAXSIZE];
int matrixR[MAXSIZE][MAXSIZE];

void *worker(void *);

int main(int argc, char *argv[]) {

    int i, j;

    size = atoi(argv[1]);

    if(size > MAXSIZE) {
        printf("maximum matrix size is %d!\n", MAXSIZE);
        return 0;
    }

    pthread_t workerid[size];

    pthread_barrier_init(&barrier, NULL, size);

    // inicializando valores da matriz 1
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            matrix1[i][j] = 1;
        }
    }

    // inicializando valores da matriz 2
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            matrix2[i][j] = 2;
        }
    }

    // inicializando valores da matriz resultado
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            matrixR[i][j] = 0;
        }
    }

    int * id;

    for (i = 0; i < size; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&workerid[i], NULL, worker, (void *) (id));
    }

    for (i = 0; i < size; i++){
        if(pthread_join(workerid[i], NULL)){
            printf("\n ERROR joining thread");
            exit(1);
        }
    }

    printf("\n");
}

void *worker(void *arg) {
    int myid = *(int *)(arg);
    int j, k, l;

    int self = pthread_self();

    printf("worker %d (pthread id %d) has started\n", myid, self);

    // Exemplo de matrizes
    //
    // 1 1 1     9 9 9   worker 0
    // 1 1 1  =  9 9 9   worker 1
    // 1 1 1     9 9 9   worker 2
    //

    for (j = 0; j < size; j++) {
        // acumulador para posição calculada
        int acc = 0;

        // calcula a soma de todos os elementos sem alterar o resultado da
        // matriz resultado, ainda
        for (k = 0; k < size; k++) {
            for (l = 0; l < size; l++) {
                acc += matrix1[k][l];
            }
        }

        // atribui o valor acumulado, computado anteriormente, à posição
        // correspondente ao ID do worker e de j.
        matrixR[myid][j] = acc;
    }

    pthread_barrier_wait(&barrier);

    if (myid == 0) {
        printf("\n");
        for (j = 0; j < size; j++) {
            for (k = 0; k < size; k++) {
                printf("%d ", matrixR[j][k]);
            }
            printf("\n");
        }
    }
}

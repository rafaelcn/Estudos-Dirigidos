/**
 * Rafael Campos Nunes - 19/0098295
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// É interessante brincar com esses números de produtores e consumidores para
// observar o comportamento do algoritmo. É observado quase um equilibrio se o
// número de consumidores for igual ao número de produtores. Caso contrário, a
// produção ou o consumo sempre serão enviesados para um dos lados.

#define PR 13 //número de produtores
#define CN 10 // número de consumidores
#define N 25  //tamanho do buffer

void * produtor(void * meuid);
void * consumidor (void * meuid);

// Variáveis utilizadas no programa

int food[N];   // o buffer de comida em si
int count = 0; // o count age como indexador do buffer de comida

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // região de exclusão mútua
pthread_cond_t enough = PTHREAD_COND_INITIALIZER; // condição de buffer suficiente

int main(int argc, char *argv[]) {

    int erro;
    int i, n, m;
    int *id;

    pthread_t producers[PR];
    pthread_t consumers[CN];

    // inicializando mutexes e condições
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&enough, NULL);

    for (i = 0; i < PR+CN; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;

        if ((i & 1) == 0) {
            erro = pthread_create(&producers[i], NULL, produtor, (void*) (id));
        } else {
            erro = pthread_create(&consumers[i], NULL, consumidor, (void*) (id));
        }

        if(erro) {
            printf("erro na criacao do thread %d\n", i);
            exit(1);
        }
    }

    pthread_join(producers[0], NULL);
}

void * produtor(void* id) {

    while (1) {
        int n = *(int*) id;

        pthread_mutex_lock(&lock);
            // enquanto o indexador do buffer de comida for menor que o tamanho
            // do próprio buffer, essa região crítica preenche uma unidade do
            // buffer de comida.
            if (count < N) {
                int x = rand() % 10;

                food[count] = x;
                count++;

                printf("produtor %d produziu. comida restante %d\n", n, count);
            }
        pthread_mutex_unlock(&lock);

        // indique aos consumidores que o buffer tem comida para ser
        // consumida. Esse broadcast tem que ser feito após o cozinheiro não
        // estiver com o mutex da comida. Caso contrário, o consumidor nunca
        // conseguirá realizar sua tarefa
        pthread_cond_broadcast(&enough);
        // esse sleep é necessário para implementar algum tipo de justiça no
        // mutex de comida.
        sleep(1);
    }

    pthread_exit(0);
}

void * consumidor(void* id) {

    while (1) {
        pthread_mutex_lock(&lock);
            int n = *(int*) id;

            while (count <= 0) {
                // comida insuficiente, esperar até ter comida suficiente
                printf("consumidor %d não pode comer\n", n);
                // aguardar até que tenha comida suficiente para consumir
                pthread_cond_wait(&enough, &lock);
            }

            count--;
            printf("consumidor %d consumiu. restante %d\n", n, count);
        pthread_mutex_unlock(&lock);
        sleep(1);
    }

    pthread_exit(0);
}

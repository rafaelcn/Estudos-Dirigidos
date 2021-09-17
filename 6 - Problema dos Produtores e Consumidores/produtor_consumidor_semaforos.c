#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PR 5  // producers quantity
#define CN 15 // consumers quantity
#define N 10  // buffer size


// variáveis utilizadas para implementar o fluxo de produtor consumidor.

sem_t s_buffer;
pthread_mutex_t m_buffer;

int buffer[N];
int buffer_index = 0;

// funções que abstraem o comportamento das entidades do problema.

void *produtor(void *id);
void *consumidor(void *id);

int main(int argc, char **argv)
{
    int erro;
    int i, n, m;
    int *id;

    // inicializa o semáforo com o valor máximo estimado de utilização, no caso,
    // é o tamanho do buffer.
    sem_init(&s_buffer, 0, N);

    pthread_mutex_init(&m_buffer, NULL);

    pthread_t threads_producer[PR];

    for (i = 0; i < PR; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        erro = pthread_create(&threads_producer[i], NULL, produtor, (void *)(id));

        if (erro)
        {
            printf("error creating thread %d\n", i);
            exit(1);
        }
    }

    pthread_t threads_consumer[CN];

    for (i = 0; i < CN; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        erro = pthread_create(&threads_consumer[i], NULL, consumidor,
            (void *)(id));

        if (erro)
        {
            printf("error creating thread %d\n", i);
            exit(1);
        }
    }

    pthread_join(threads_producer[0], NULL);

    sem_destroy(&s_buffer);
    pthread_mutex_destroy(&m_buffer);

    return 0;
}

void *produtor(void *id) {
    while (1) {
        int id_ = *(int*) id;
        pthread_mutex_lock(&m_buffer);
            // somente preencher o buffer se ele não estiver cheio
            if (buffer_index < N) {
                buffer[buffer_index] = rand() % 10;
                buffer_index++;

                printf("produtor %d acabou de produzir... [%d]\n", id_,
                    buffer_index);
            }
        pthread_mutex_unlock(&m_buffer);
        // avisar que uma unidade de produção já está disponível aos
        // consumidores. Essa função incrementa uma unidade de valor do semáforo
        // se ele já não estiver completo (com seu valor máximo).
        sem_post(&s_buffer);
    }
    pthread_exit(0);
}

void *consumidor(void *id) {
    while (1) {
        int id_ = *(int*) id;
        // aguarde o semáforo do buffer para consumo, isto é, aguardar o valor
        // do semáforo ser maior do que 0.
        sem_wait(&s_buffer);
            pthread_mutex_lock(&m_buffer);
                if (buffer_index > 0) {
                    int consumo = buffer[buffer_index];
                    buffer_index--;
                }
            pthread_mutex_unlock(&m_buffer);
        sem_post(&s_buffer);

        printf("consumidor %d está ingerindo... [%d]\n", id_, buffer_index);
        sleep(rand() % 3);
    }
    pthread_exit(0);
}

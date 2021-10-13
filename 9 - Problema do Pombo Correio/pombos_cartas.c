#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários
#define N_LETTERS 20 //quantidade de cartas na mochila

int bag = 0;


sem_t s_bag;
sem_t s_pigeon_wakeup;

pthread_mutex_t m_bag = PTHREAD_MUTEX_INITIALIZER;

void * f_user(void *arg);
void * f_pigeon(void *arg);

int main(int argc, char **argv) {

    int i;
    int *id;

    pthread_t usuario[N];

    sem_init(&s_bag, 0, N_LETTERS);
    sem_init(&s_pigeon_wakeup, 0, 0);

    for(i = 0; i < N; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	    pthread_create(&(usuario[i]), NULL, f_user, (void *) (id));
    }

    pthread_t pombo;

    id = (int *) malloc(sizeof(int));
    *id = 0;

    pthread_create(&(pombo), NULL, f_pigeon, (void*) (id));
    pthread_join(pombo, NULL);
}


void * f_pigeon(void *arg){

    while (1) {
        // Inicialmente está em A, aguardar/dorme a mochila ficar cheia (20
        // cartas)
        printf("pombo está em A\n");
        sem_wait(&s_pigeon_wakeup);

        pthread_mutex_lock(&m_bag);
            // Leva as cartas para B e volta para A
            printf("pombo leva cartas para B e volta\n");
            sleep(2);

            // Esvaziar mochila, acordar os usuários e  devolver permissões para
            // os usuários
            while (bag > 0) {
                sem_post(&s_bag);
                bag--;
            }
        pthread_mutex_unlock(&m_bag);
    }
}

void * f_user(void *arg){

    while (1) {
        int id = * (int*) arg;

        // Aguarda por permissões sobre a mochila do pombo
        sem_wait(&s_bag);

        // Região crítica onde somente uma entidade pode alterar a variável bag
        pthread_mutex_lock(&m_bag);
            // Posta sua carta na mochila do pombo
            if (bag < N_LETTERS) {
                bag++;
                printf("%d escreveu uma carta... %d\n", id, bag);
                sleep(rand()%1);
            }

            // Caso a mochila fique cheia, acorda o pombo
            if (bag >= N_LETTERS) {
                printf("%d preparando pombo para envio...\n", id);
                sem_post(&s_pigeon_wakeup);
            }
        pthread_mutex_unlock(&m_bag);
    }
}

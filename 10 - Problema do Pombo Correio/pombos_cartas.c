#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários
#define N_LETTERS 20 //quantidade de cartas na mochila

int bag = 0;

pthread_mutex_t m_bag = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t c_pigeon = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_writer = PTHREAD_COND_INITIALIZER;

void * f_user(void *arg);
void * f_pigeon(void *arg);

int main(int argc, char **argv) {

    int i;
    int *id;

    pthread_t usuario[N];

    for(i = 0; i < N; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	    pthread_create(&(usuario[i]), NULL, f_user, (void *) (id));
    }

    pthread_t pombo;

    id = (int *) malloc(sizeof(int));
    *id = 0;

    pthread_create(&(pombo), NULL, f_pigeon, (void*) (id));

    pthread_join(pombo,NULL);
}


void * f_pigeon(void *arg){

    while (1) {
        // Inicialmente está em A, aguardar/dorme a mochila ficar cheia
        printf("pombo está em A\n");

        pthread_mutex_lock(&m_bag);
            while (bag < N_LETTERS) {
                pthread_cond_broadcast(&c_writer);
                pthread_cond_wait(&c_pigeon, &m_bag);
            }

            // Leva as cartas para B e volta para A
            // Acordar os usuários
            printf("pombo leva cartas para B e volta para A\n");

            while (bag > 0) {
                --bag;
            }
            
            // simulação de tempo que o pombo levou para descarregar cartas e
            // voltar ao ponto A.
            sleep(1);
        pthread_mutex_unlock(&m_bag);
    }
}

void * f_user(void *arg){

    while (1) {
        int id = * (int*) arg;

        pthread_mutex_lock(&m_bag);
            //Caso o pombo não esteja em A ou a mochila estiver cheia, então
            //dorme
            while (bag >= N_LETTERS) {
                //Caso a mochila fique cheia, acorda o pombo
                pthread_cond_signal(&c_pigeon);
                pthread_cond_wait(&c_writer, &m_bag);
            }

            //Escreve uma carta
            if (bag < N_LETTERS) {
                //Posta sua carta na mochila do pombo
                ++bag;
                printf("%d escreveu uma carta... %d\n", id, bag);
            }
        pthread_mutex_unlock(&m_bag);

        // a variabilidade de usuários escrevendo cartas a cada vez que o pombo
        // está em A é maior com esse sleep. Retire para visualizar o outro
        // comportamento. Isso provavelmente se deve ao fato de que o
        // escalonador do meu sistema operacional oferece o recurso de CPU à
        // thread que já está com os recursos próximos ao processador (cache) e
        // solicitar acesso de computação. Com o sleep é possível obrigar o
        // escalonador a escolher outra thread que solicita  acesso ao recurso
        // de computação para a CPU. Isso não resolve o problema, somente traz
        // variedade à saída.
        sleep(rand()%2);
    }
}

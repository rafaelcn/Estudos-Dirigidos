#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_CLIENTS 15
#define N_CHAIRS 4

sem_t s_chairs;
sem_t s_barber_chair;
sem_t s_barber_wakeup;
sem_t s_client_wakeup;

void * f_barber(void *v) {

  while (1) {
    // esperar/dormindo algum cliente sentar na cadeira do barbeiro (e acordar o
    // barbeiro)
    sem_wait(&s_barber_wakeup);

    sleep(1+rand()%2); //Cortar o cabelo do cliente
    printf("Barbeiro cortou o cabelo de um cliente\n");

    // liberar/desbloquear o cliente
    sem_post(&s_client_wakeup);
  }

  pthread_exit(0);
}

void* f_client(void* v) {
    int id = *(int*) v;
    sleep(id%3+1);

    // verifica se há a possibilidade de adquirir um assento na barberia
    if (sem_trywait(&s_chairs) == 0) {
        // conseguiu pegar uma cadeira de espera
        printf("Cliente %d entrou na barbearia \n",id);
        // pegar/sentar a cadeira do barbeiro
        sem_wait(&s_barber_chair);
        // liberar a sua cadeira de espera
        sem_post(&s_chairs);
        // acordar o barbeiro para cortar seu cabelo
        sem_post(&s_barber_wakeup);
        // aguardar o corte do seu cabelo
        sem_wait(&s_client_wakeup);
        // liberar a cadeira do barbeiro
        sem_post(&s_barber_chair);
        printf("Cliente %d cortou o cabelo e foi embora \n",id);
    } else {
        // barbearia cheia
        printf("Barbearia cheia, cliente %d indo embora\n",id);
    }

    pthread_exit(0);
}

int main(int argc, char** argv) {

  pthread_t thr_clientes[N_CLIENTS], thr_barbeiro;
  int i, id[N_CLIENTS];

  sem_init(&s_chairs, 0, N_CHAIRS);
  sem_init(&s_barber_chair, 0, 1);
  sem_init(&s_barber_wakeup, 0, 1);
  sem_init(&s_client_wakeup, 0, 1);

  for (i = 0; i < N_CLIENTS; i++) {
    id[i] = i;
    pthread_create(&thr_clientes[i], NULL, f_client, (void*) &id[i]);
  }

  pthread_create(&thr_barbeiro, NULL, f_barber, NULL);

  for (i = 0; i < N_CLIENTS; i++) {
    pthread_join(thr_clientes[i], NULL);
  }

  /* Barbeiro assassinado */

  return 0;
}


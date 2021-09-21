/**
 * Rafael Campos Nunes - 19/0098295
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

// os locks abaixo definem recursos que devem ser mutualmente exclusivos entre
// os atores do programa. A corda e a prioridade são recursos compartilhados
// entre todos os atores - macacos de AB e de macacos de BA, além dos gorilas.
pthread_mutex_t lock_rope = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_priority = PTHREAD_MUTEX_INITIALIZER;

// por fim, esses locks definem acesso mutualmente exclusivo entre os contadores
// de macacos de cada morro.
pthread_mutex_t lock_count_ab = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_count_ba = PTHREAD_MUTEX_INITIALIZER;

// os contadores abaixo definem a quantidade de macacos que passaram pela corda,
// em suas respectivas direções.
int count_ab = 0;
int count_ba = 0;

// o contador abaixo é definido para que o gorila não monipolize o recurso
// corda, ou seja, é definido um tempo limite para a quantidade de vezes que o
// gorila pode passar pelo recurso corda.
int count_gorilla = 0;

/**
 * Os procedimentos de entrada e saída da corda, para os macacos do morro A e B
 * são muito similares, com a diferença nos contadores utilizados. Por esse
 * motivo não comentei a solução em particular para ambos os casos, até por ser
 * idêntica à solução utilizada no problema dos leitores e escritores.
 */

void* macacoAB(void * a) {
    int i = *((int *) a);
    while (1) {
        pthread_mutex_lock(&lock_priority);
            // Procedimentos para acessar a corda
            pthread_mutex_lock(&lock_count_ab);
                count_ab++;
                if (count_ab == 1) {
                    pthread_mutex_lock(&lock_rope);
                }
            pthread_mutex_unlock(&lock_count_ab);
        pthread_mutex_unlock(&lock_priority);

        printf("Macaco %d passando de A para B \n", i);
        sleep(1);

	    // Procedimentos para quando sair da corda
        pthread_mutex_lock(&lock_count_ab);
        count_ab--;
        if (count_ab == 0) {
            pthread_mutex_unlock(&lock_rope);
        }
        pthread_mutex_unlock(&lock_count_ab);
    }
    pthread_exit(0);
}

void* macacoBA(void * a) {
    int i = *((int *) a);
    while (1) {
        pthread_mutex_lock(&lock_priority);
            // Procedimentos para acessar a corda
            pthread_mutex_lock(&lock_count_ba);
                count_ba++;
                if (count_ba == 1) {
                    pthread_mutex_lock(&lock_rope);
                }
            pthread_mutex_unlock(&lock_count_ba);
        pthread_mutex_unlock(&lock_priority);

        printf("Macaco %d passando de B para A \n", i);
        sleep(1);

        // Procedimentos para quando sair da corda
        pthread_mutex_lock(&lock_count_ba);
            count_ba--;
            if (count_ba == 0) {
                pthread_mutex_unlock(&lock_rope);
            }
        pthread_mutex_unlock(&lock_count_ba);
    }
    pthread_exit(0);
}

void* gorila(void * a) {
    // O procedimento para garantir uma forma de justiça, na passagem do gorila,
    // é utilizar um lock de prioridade. Além disso, o gorila só pode passar
    // quando o contador for um múltiplo de 5, para que o gorila não monopolize
    // o recurso corda.
    while (1) {
        sleep(rand() % 5);

        // Se um ator obtiver lock sobre a preferência, ele tem também acesso ao
        // recurso corda, de modo exclusivo.
        pthread_mutex_lock(&lock_priority);
            // Sobre o domínio do lock de prioridade, o código abaixo realiza o
            // incremento do contador e o gorila só ganha acesso ao recurso de
            // corda se o contador é maior que 5, onde será zerado e o gorila
            // realizará a travessia. Nesse sentido, é dada uma prioridade muito
            // maior aos macacos do que ao gorila.
            pthread_mutex_lock(&lock_rope);
                printf("Gorila passando de A para B \n");
                sleep(2);
            pthread_mutex_unlock(&lock_rope);
            // Procedimentos para quando sair da corda
        pthread_mutex_unlock(&lock_priority);
    }
    pthread_exit(0);
}

int main(int argc, char * argv[])
{
    pthread_t macacos[MA+MB];
    int *id;

    for (int i = 0; i < MA+MB; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;

        if (i%2 == 0) {
          if (pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)) {
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        } else {
          if (pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)) {
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }
    }

    pthread_t g;
    pthread_create(&g, NULL, &gorila, NULL);

    pthread_join(macacos[0], NULL);

    return 0;
}

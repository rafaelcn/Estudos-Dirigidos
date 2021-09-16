/**
 * Rafael Campos Nunes - 19/0098295
 */

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define TRUE 1

#define NE 3 //numero de escritores
#define NL 10 //numero de leitores

pthread_mutex_t lock_bd = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_nl = PTHREAD_MUTEX_INITIALIZER;

// Para resolver o problema de starvation, algum tipo de mecanismo deve obrigar
// a thread ~bloqueada~ por muito tempo, a ser executada. Isso pode ser feito
// com semáforos, condições de espera, mutexes etc.

pthread_mutex_t lock_priority = PTHREAD_MUTEX_INITIALIZER;

int num_escritas = 0;
int num_leituras = 0;

int num_leitores = 0;

void * reader(void *arg);
void * writer(void *arg);
void read_data_base();
void use_data_read();
void think_up_data();
void write_data_base();

int main() {

	pthread_t r[NL], w[NE];
	int i;
    int *id;

    /* criando leitores */
    for (i = 0; i < NL ; i++) {
	    id = (int *) malloc(sizeof(int));
        *id = i;
		pthread_create(&r[i], NULL, reader, (void *) (id));
	}

	/* criando escritores */
	for (i = 0; i< NE; i++) {
	   id = (int *) malloc(sizeof(int));
           *id = i;
		 pthread_create(&w[i], NULL, writer, (void *) (id));
	}

	pthread_join(r[0], NULL);
	return 0;
}

void * reader(void *arg) {
	int i = *((int *) arg);
    // repete para sempre
	while (TRUE) {
        // bloqueia a thread atual para implementar um mecanismo de justiça
        // (fairness) caso o escritor já tenha posse desse mutex.
        pthread_mutex_lock(&lock_priority);
        pthread_mutex_unlock(&lock_priority);

        pthread_mutex_lock(&lock_nl);
            num_leitores++;
            if (num_leitores == 1) {
                pthread_mutex_lock(&lock_bd);
            }
        pthread_mutex_unlock(&lock_nl);

        // acesso à base de dados
        read_data_base(i);

        pthread_mutex_lock(&lock_nl);
            num_leituras++;
            num_leitores--;

            if (num_leitores == 0) {
                pthread_mutex_unlock(&lock_bd);
            }
        pthread_mutex_unlock(&lock_nl);

        // região não crítica
        use_data_read(i);
	}

    pthread_exit(0);
}

void * writer(void *arg) {
	int i = *((int *) arg);
    // repete para sempre
	while (TRUE) {
        // região não crítica
		think_up_data(i);

        // realiza o bloqueio do mutex de prioridade, para possibilitar o
        // escritor um acesso mais frequente ao recurso e não ser bloqueado,
        // como acontecia antes desse implementação. Isso pode ser verificado
        // analisando a quantidade de vezes que o recurso foi escrito/lido, pois
        // o resultado dessa razão é aproximadamente a razão de
        // escritores/leitores.
        pthread_mutex_lock(&lock_priority);
            pthread_mutex_lock(&lock_bd);
                // atualização da base de dados
                write_data_base(i);
                num_escritas++;
            pthread_mutex_unlock(&lock_bd);
        pthread_mutex_unlock(&lock_priority);

        // Para verificar basta deixar o programa rodando por um tempo e pegar
        // os dois últimos números da n-upla [x, y, z], ou seja, y e z e
        // calcular a razão entre eles, z/y ~ 30%.
    }
    pthread_exit(0);
}

/**
 * A seção abaixo foi modificada para simplificar a saída e mostrar mais dados.
 * O formato de saída é claro, o número de leitores, de leituras e número de
 * vezes em que o banco de dados foi escrito.
 */

void read_data_base(int i) {
	printf("RE[%d] (reading)  [%d, %d, %d]\n", i, num_leitores, num_leituras, num_escritas);
	sleep(rand() % 5);
}

void use_data_read(int i) {
	printf("RE[%d] (using)    [%d, %d, %d]\n", i, num_leitores, num_leituras, num_escritas);
	sleep(rand() % 5);
}

void think_up_data(int i) {
	printf("WR[%d] (thinking) [%d, %d, %d]\n", i, num_leitores, num_leituras, num_escritas);
	sleep(rand() % 5);
}

void write_data_base(int i) {
	printf("WR[%d] (writing)  [%d, %d, %d]\n", i, num_leitores, num_leituras, num_escritas);
	sleep(rand() % 10);
}

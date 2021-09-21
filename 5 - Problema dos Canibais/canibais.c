/**
 * Rafael Campos Nunes - 19/0098295
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/**
 * Restrições:
 *
 * 1. Canibais não podem servir ao mesmo tempo
 * 2. Canibais não podem comer ao mesmo tempo
 * 3. Canibais não podem se servir se o caldeirão estiver vazio
 * 4. Canibais só podem se servir após o cozinheiro terminar de preparar o
 *    jantar (as porções)
 * 5. Se o caldeirão estiver vazio, os canibais esperam dormindo
 * 6. O cozinheiro só pode encher o caldeirão quando o caldeirão estiver vazio
 */


#define MAXCANIBAIS 20

void *canibal(void* id);
// modifiquei a função para receber a quantidade de canibais
void *cooker(int m, int n);

// Variáveis utilizadas no programa

pthread_cond_t to_cook = PTHREAD_COND_INITIALIZER;
pthread_cond_t to_eat = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int food = 0;

int main(int argc, char *argv[]) {

    int erro;
    int i, n, m;
    int *id;

    pthread_t tid[MAXCANIBAIS];

    if (argc != 3) {
        printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
        exit(1);
    }

    n = atoi (argv[1]); // número de canibais
    m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue
    // preparar por vez

    printf("número de canibais: %d | qtd. produzida por vez: %d\n", n, m);

    if (n > MAXCANIBAIS) {
        printf("o numero de canibais é maior que o maximo permitido: %d\n",
            MAXCANIBAIS);
        exit(1);
    }

    for (i = 0; i < n; i++)  {
        id = (int *) malloc(sizeof(int));
        *id = i;

        erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

        if (erro) {
            printf("erro na criação da thread %d\n", i);
            exit(1);
        }
    }

    cooker(m, n);

    pthread_mutex_destroy(&lock);
}

void * canibal(void* pi) {

    while (1) {
        int id = *((int *) pi);

        //pegar uma porção de comida e acordar o cozinheiro se as porções
        //acabaram
        pthread_mutex_lock(&lock);
            // esse laço, apesar de parecer repetitivo, deve existir pois, caso
            // contrário, pode não haver comida para o canibal comer e se eu
            // colocasse a espera (pthread_cond_wait) abaixo da linha 105 o
            // canibal iria sair da região crítica assim que recebesse a ordem
            // para comer.
            while (food == 0) {
                printf("%d: sem comida, vou acordar o cozinheiro...\n", id);
                // avisa ao cozinheiro que a condição to_cook é verdadeira e
                // ele deve iniciar a preparação da comida.
                pthread_cond_signal(&to_cook);
                // aguarda o sinal do cozinheiro de que é hora de comer!
                pthread_cond_wait(&to_eat, &lock);
            }


            printf("%d: vou pegar uma porção\n", id);
            food--;

            if (food == 0) {
                pthread_cond_signal(&to_cook);
            }
        pthread_mutex_unlock(&lock);

        // fora da região crítica
        printf("%d: vou comer a porcao que peguei, comida restante %d\n", id,
            food);
        sleep(5);
    }
}

void *cooker(int m, int n) {

    while (1) {
        pthread_mutex_lock(&lock);
            // o cozinheiro só pode cozinhar se não houver comida alguma no
            // caldeirão.
            if (food > 0) {
                pthread_cond_wait(&to_cook, &lock);
            }

            printf("cozinheiro: vou cozinhar\n");
            while (food < n) {
                food += m;
                printf("cozinheiro: produzindo... produzido %d\n", food);
                sleep(rand() % 3);
            }
            printf("cozinheiro: ok, agora acordando os canibais e indo dormir\n");

            // acordar os canibais
            pthread_cond_broadcast(&to_eat);
        pthread_mutex_unlock(&lock);

        // dormir
        sleep(10);
    }
}

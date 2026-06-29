#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define NUM_PESSOAS 8 //numero de pessoas e thread

//struct de cada pessoa para controlar a fila
typedef struct{
    int id;
    char nome[50];
    int prioridadeinicio;
    int prioridadeatual;
    int iteracoes;
}clientes;

clientes pessoas[NUM_PESSOAS]; //vetor da struct
pthread_t threads[NUM_PESSOAS];

//vetor para quardar prioridade de cada pessoa
int prioridades[NUM_PESSOAS] =
{
    1, // maria
    1, // marcos
    2, // vanda
    2, // valter
    3, // paula
    3, // pedro
    4, // sueli
    4  // silas
};

//funçao para criar cada cliente
void criarpessoas(int iteracoes) {
    char *nomes[NUM_PESSOAS] = {
        "Maria", "Marcos", "Vanda", "Valter", "Paula", "Pedro",  "Sueli", "Silas"
    };

    for (int i = 0; i < NUM_PESSOAS; i++) {
        pessoas[i].id = i;
        strcpy(pessoas[i].nome, nome[i]); 
        pessoas[i].prioridadeinicio = prioridades[i];
        pessoas[i].prioridadeatual = prioridades[i];
        pessoas[i].iteracoes = iteracoes;
    }
}

void caixa(clientes *p)
{
    printf("%s esta sendo atendido(a).\n", p->nome);
    sleep(1);
}

//descanso de 3 segundos
void descanso()
{
    sleep(3); 
}

//pessoa sendo atendida
void *atenderpessoa(void *arg) {
    clientes *p = (clientes *)arg;

    for (int i = 0; i < p->iteracoes; i++) {
        printf("%s chegou na loterica.\n", p->nome);
        caixa(p);
        printf("%s vai para casa.\n", p->nome);
        descanso();
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Numero de argumentos invalidos.");
        return 1;
    }

    int iteracoes = atoi(argv[1]);

    criarpessoas(iteracoes);

    for(int i=0; i < NUM_PESSOAS; i++)
    {
        pthread_create(&threads[i], NULL, atenderpessoa, &pessoas[i]);
    }

    for(int i=0; i< NUM_PESSOAS; i++) //aguarda para finaliza de thread
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}


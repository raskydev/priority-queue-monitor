#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

//struct do monitor, quem controla quem acessa o caixa
typedef struct{
    pthread_mutex_t trava; //mutex para exclusao 
    pthread_cond_t cond_pessoas[8];
    int na_fila[8]; //1 se à uma pessoa esperando na fila 
    int furos_de_fila[8];
    int caixa_ocupado; //1 se o caixa estiver ocupado
    int pessoas_ativas;
} monitor_caixa_t;

monitor_caixa_t monitor;
typedef struct{
    int id;
    char nome[50];
    int prioridadeinicio;
    int prioridadeatual;
    int iteracoes;
} clientes;

extern clientes pessoas[8];

int escolher_proximo_cliente(); 

//funçao para a pessoa que entrar na fila esperar sua vez de ser atendida

void espera(int id) {
    pthread_mutex_lock(&monitor.trava); //entra na seçao critica
    
    monitor.na_fila[id] = 1;
    printf("%s está na fila do caixa\n", pessoas[id].nome);
     
    //aguarda enquanto nao for a vez da pessoa ou enquanto o caixa estiver ocupado
    while (monitor.caixa_ocupado || escolher_proximo_cliente() != id) {
        pthread_cond_wait(&monitor.cond_pessoas[id], &monitor.trava);
    }
    
    monitor.caixa_ocupado = 1; //caixa ocupado
    monitor.na_fila[id] = 0; //saiu da fila
    
    pthread_mutex_unlock(&monitor.trava);//saçao critica liberada
}
 //pessoa libera o caixa apos ser atendida
void libera(int id){
    pthread_mutex_lock(&monitor.trava);
    monitor.caixa_ocupado = 0;

    //acorda todas as threads para o escalonador escolher a proxima pessoa

    for (int i = 0; i < 8; i++ ){ 
        pthread_cond_signal (&monitor.cond_pessoas[i]);
    }
    
    pthread_mutex_unlock(&monitor.trava);
}

//inicia o monitor antes de criar as threads
void monitor_init(){
    pthread_mutex_init(&monitor.trava, NULL);
    
    for (int i = 0; i < 8; i++) {
        pthread_cond_init(&monitor.cond_pessoas[i], NULL); //inicia cada variavel de condiçao
    }
    monitor.caixa_ocupado = 0;
    monitor.pessoas_ativas = 8;
}

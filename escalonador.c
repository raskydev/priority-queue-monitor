#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PESSOAS 8

//definicao das estruturas 
typedef struct {
    int id;
    char nome[50];
    int prioridadeinicio;
    int prioridadeatual;
    int iteracoes;
} clientes;

typedef struct {
    pthread_mutex_t trava;
    pthread_cond_t cond_pessoas[NUM_PESSOAS];
    int na_fila[NUM_PESSOAS];           
    int furos_de_fila[NUM_PESSOAS];     
    int caixa_ocupado;                  
    int pessoas_ativas;                 
} monitor_caixa_t;

//pega as variaveis globais do main
extern clientes pessoas[NUM_PESSOAS];
extern monitor_caixa_t monitor;

//escalonador / inaniçao 
int escolher_proximo_cliente() {
    int proximo_id = -1;
    
    //inaniçao
    //serve para evitar que pessoas de menos prioridade fiquem eternamente na fila sendo furadas o tempo todo, aumentando
    //temporariamente sua prioridade 
    for (int i = 0; i < NUM_PESSOAS; i++) {
        if (monitor.na_fila[i] && monitor.furos_de_fila[i] >= 2) { //verifica o estado da fila e quantas vezes já foi ultrapassado por outros
            if (pessoas[i].prioridadeatual > 1) { //só aumenta a prioridade se ela não for máxima (1) 
                pessoas[i].prioridadeatual--; //diminui o valor da prioridade atual para aumentar o nível (é decrescente) 
            }
            monitor.furos_de_fila[i] = 0; //reseta o contador de quem tiver acabado de ser beneficiado 
            printf("Gerente detectou inanição, aumentando prioridade de %s\n", pessoas[i].nome);
        }
    }

    //seleçao por prioridade 
    //compara as prioridades dos integrantes da fila e prioriza o menor valor de prioridade (porque é
    //decrescente, 1 tem maior prioridade que 4)
    int melhor_prioridade = 999; 
    for (int i = 0; i < NUM_PESSOAS; i++) {
        if (monitor.na_fila[i]) { //verifica se a pessoa [i] está na fila do caixa 
            if (pessoas[i].prioridadeatual < melhor_prioridade) { //compara as prioridades 
                                                                  //número menor = maior prioridade
                melhor_prioridade = pessoas[i].prioridadeatual; //sendo menor, passa a ser a melhor prioridade
                proximo_id = i; //guarda o id da prioridade maior para ser o próximo atendido 
            }
        }
    }

    //computa furos de fira para os restantes
    //usa o parametro de prioridade inicial para analisar quando alguem perde a vez para outra pessoa que é originalmente
    //mais prioritária
    if (proximo_id != -1) { //vê se existe alguém para ser atendido 
        for (int i = 0; i < NUM_PESSOAS; i++) {
            if (monitor.na_fila[i] && i != proximo_id) {
                if (pessoas[i].prioridadeinicio > pessoas[proximo_id].prioridadeinicio) { //compara a prioridades inicias para
                                                                                          //definir furos de fila que aconteceram
                    monitor.furos_de_fila[i]++;
                }
            }
        }
    }

    return proximo_id;
}

//detecta deadlock
void gerente_detectar_deadlock() {
    int tem_gravida = monitor.na_fila[0] || monitor.na_fila[1];
    int tem_idoso   = monitor.na_fila[2] || monitor.na_fila[3];
    int tem_defic   = monitor.na_fila[4] || monitor.na_fila[5];

    if (monitor.caixa_ocupado == 0 && tem_gravida && tem_idoso && tem_defic) { //se o caixa estiver livre e tiver pelo menos uma pessoa
                                                                               //pessoa de cada categoria esperando significa que ocorreu
                                                                               //um deadlock e ninguém pode avançar sozinho
        int escolhido = -1;
        while (escolhido == -1) {
            int r = rand() % 6;  //sorteia alguém aleatório na fila para ser atendido
            if (monitor.na_fila[r]) {
                escolhido = r;
            }
        }
        printf("Gerente detectou deadlock, liberando %s para atendimento\n", pessoas[escolhido].nome);
        
        monitor.caixa_ocupado = 1; //caixa ocupado pelo sorteado
        monitor.na_fila[escolhido] = 0;
        pthread_cond_signal(&monitor.cond_pessoas[escolhido]);
    }
}

//thread do gerente
void *thread_gerente_run(void *arg) {
    while (monitor.pessoas_ativas > 0) {
        sleep(5); 
        
        pthread_mutex_lock(&monitor.trava);
        gerente_detectar_deadlock();
        pthread_mutex_unlock(&monitor.trava);
    }
    return NULL;
}

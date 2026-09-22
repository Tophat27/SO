#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N_leitoras 5
#define M_escritoras 5
#define Tamanho_array 100


typedef struct {
    int *array;
    int id; // Identificador da thread escritora

}EscritoraArgs;

typedef struct {
    int *array;
    int id; // Identificador da thread leitora

}LeitoraArgs;

int *Array;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pode_ler = PTHREAD_COND_INITIALIZER;
pthread_cond_t pode_escrever = PTHREAD_COND_INITIALIZER;

int leitores_ativos = 0;
int escritores_ativos = 0;
int escritores_esperando = 0;

void *leitora(void* args){
    LeitoraArgs *leitor = (LeitoraArgs*) args;

    while(1){

        pthread_mutex_lock(&mutex);
        while (escritores_ativos > 0 || escritores_esperando > 0) {
            pthread_cond_wait(&pode_ler, &mutex);
        }
        leitores_ativos++;
        pthread_mutex_unlock(&mutex);

        int posicao = rand() % Tamanho_array;
        int valor = leitor->array[posicao];
        printf("A Thread Leitora %d leu o valor %d na posição %d\n", leitor->id, valor, posicao);
        
        pthread_mutex_lock(&mutex);
        leitores_ativos--;
        if (leitores_ativos == 0) {
            pthread_cond_signal(&pode_escrever);
        }
        pthread_mutex_unlock(&mutex);
    } 
}

void *escritora(void* args){
    
    EscritoraArgs *escritor = (EscritoraArgs*) args;

    while(1){
        pthread_mutex_lock(&mutex);
        escritores_esperando++;

        while (leitores_ativos > 0 || escritores_ativos > 0) {
            pthread_cond_wait(&pode_escrever, &mutex);
        }
        
        escritores_esperando--;
        escritores_ativos++;
        pthread_mutex_unlock(&mutex);

        int posicao = rand() % Tamanho_array;
        int valor = rand() % 100;
        escritor->array[posicao] = valor;
        printf("A Thread Escritora %d escreveu o valor %d na posição %d\n", escritor->id, valor, posicao);
        
        pthread_mutex_lock(&mutex);
        escritores_ativos--;
        if (escritores_esperando > 0) {
            pthread_cond_signal(&pode_escrever);
        } else {
            pthread_cond_broadcast(&pode_ler);
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main(){
    Array = (int*)malloc(Tamanho_array * sizeof(int));
    pthread_t Tleitoras[N_leitoras];
    pthread_t Tescritoras[M_escritoras];

    pthread_mutex_init(&mutex, NULL);
    

    EscritoraArgs escritoras[M_escritoras];
    LeitoraArgs leitoras[N_leitoras];

    ////////////// objetos de teste
    for (int i = 0; i < M_escritoras; i++) {
        escritoras[i].array = Array;
        escritoras[i].id = i;

    }

    for (int i = 0; i < N_leitoras; i++) {
        leitoras[i].array = Array;
        leitoras[i].id = i;
    }

    int rc;
    for (int i = 0; i < N_leitoras; i++) {
        rc = pthread_create(&Tleitoras[i], NULL, leitora, (void*) &leitoras[i]);
        if (rc) {
            printf("Erro ao criar thread leitora %d\n", i);
            exit(1);
        }
    }
    for (int i = 0; i < M_escritoras; i++) {
        rc = pthread_create(&Tescritoras[i], NULL, escritora, (void*) &escritoras[i]);
        if (rc) {
            printf("Erro ao criar thread escritora %d\n", i);
            exit(1);
        }
    }

    for (int i = 0; i < N_leitoras; i++) {
        pthread_join(Tleitoras[i], NULL);
    }
    for (int i = 0; i < M_escritoras; i++) {
        pthread_join(Tescritoras[i], NULL);
    }

    
}
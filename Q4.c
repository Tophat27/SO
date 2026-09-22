#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Matriz para teste

#define numIcog 4

float A[numIcog][numIcog] = {
    {10.0, 2.0, 1.0, 1.0},
    {1.0, 10.0, 2.0, 1.0},
    {1.0, 1.0, 10.0, 2.0},
    {2.0, 1.0, 1.0, 10.0}
};
float b[numIcog] = {14.0, 14.0, 14.0, 14.0};

// Valores iniciais iguais a 1
float X0[numIcog] = {1.0, 1.0, 1.0, 1.0}; //
float Xi[numIcog] = {0.0, 0.0, 0.0, 0.0};
int p = 10;
int N;
/////////////////////

pthread_barrier_t barreira;

void* jacobi(void *arg);

int main(){

    //Valor de N
    printf("Entre com o valor de N: ");
    scanf("%i", &N);
    //////////////////

    //Criar N threads e iniciar elas
    pthread_t *threads = malloc(N*sizeof(pthread_t));

    pthread_barrier_init(&barreira, NULL, N);

    for(int i = 0; i < N; i++){
        int *indice = malloc(sizeof(int));
        *indice = i;
        pthread_create(&threads[i], NULL, jacobi, (void*)indice);
    }

    for(int i = 0; i < N; i++){
        pthread_join(threads[i], NULL);
    }

    printf("\nResultado final apos %d iteracoes:\n", p);
    for (int i = 0; i < numIcog; i++) {
        printf("X[%d] = %f\n", i, X0[i]);
    }

    pthread_barrier_destroy(&barreira);
    free(threads);
    return 0;
}

void* jacobi(void *arg){
    int id = *(int*)arg;
    int k = 0;
    while (k < p){

        //a lógica desse primeiro for serve para distribuir igualmente as icógnitas as threads, intercalando a execução das threads para cada incógnita
        for(int i = id; i < numIcog; i+= N){
            float soma = 0;
            for(int j = 0; j < numIcog; j++){
                if(i != j){
                    soma += A[i][j]*X0[j];
                    //printf("soma: %f\n", soma);
                }
            }

            Xi[i] = 1.0/A[i][i] * (b[i] - soma);    
            //printf("X[i]: %f\n", Xi[i]);
            }
            pthread_barrier_wait(&barreira); //barreira para aguardar a threads calcularem seus respectivos xi
        
        for(int i = id; i < numIcog; i += N){
            X0[i] = Xi[i]; 
        }

        pthread_barrier_wait(&barreira);   //barreira para evitar que as threads sobrescrevam os x0
        k++;
    }
}

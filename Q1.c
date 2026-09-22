/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TAM 1000 //da questao
#define n_threads 2//de teste

typedef struct 
{
    int *vetor;
    int comeco;
    int fim;
    long long somador;
}Thread;
void* soma(void *arg){
    Thread *args = (Thread*) arg;
    args->somador = 0;

    for (int y = args->comeco; y < args->fim; y++) {
        args->somador += args->vetor[y];
    }

    return NULL;
}

int main(){
     pthread_t threads[n_threads];
     Thread args[n_threads];
    int *vetor = (int*)malloc(TAM * sizeof(int));
    for (int x =0 ; x < TAM;x++){
        vetor[x]=1;
    }
    int chunk = TAM/n_threads;
    for (int i = 0 ; i < n_threads; i++)
    {
        args[i].vetor = TAM;
        args[i].comeco=  i*chunk;
        if (i == n_threads-1){
            args[i].fim = TAM;
        }else{
            args[i].fim = i * chunk;
        }
        if(pthread_create(&threads[i],NULL,soma,(void*)&args[i])!=0){
            perror("algo errado com a thread");
            return 1;
        }

    }
    long long total=0;
    for(int f=0;f<TAM;f++){
        pthread_join(threads[f], NULL);
        total += args[f].somador;
    }
    
    
    printf("total: %lld", total);
    free(TAM);
    return 0;

}

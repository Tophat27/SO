#include <stdio.h>
#include <stdlib.h>
#include <pthreads.h>

#define vector 1000 //da questao
#define n_threads 2 //de teste

typedef struct 
{
    int *vetor;
    int comeco;
    int fim;
    long long somador;
}Thread;
void* soma(){

}
int main(){
    int *vector;
    p_thread_t threads[n_threads];
    args Thread[n_threads];

    vector = (int*)malloc(vector * sizeof(int));
    for (int x =0 ; x < n_threads:x++){
        vector[x]=1;
    }
    int chunk = vector/n_threads;
    for (int i = 0 ; i < n_threads; i++)
    {
        Thread[i].vetor = vector;
        Thread[i].comeco=  i*chunk;
        if (i == n_threads){
            Thread[i].fim = vector;
        }else{
            Thread[i].fim = (i+1) * chunk;
        }
        if(pthread_create(&threads[i],NULL,soma,(void*)&Thread[i])!=0){
            perror("algo errado com a thread");
            return 1;
        }

    }
    long long total=0;
    printf("total: %lld", total);
    free(vector);
    return 0;

}

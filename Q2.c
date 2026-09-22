#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    char arquivo[100]; 
} dados_threads;

int *cor;
int *texto;

pthread_mutex_t *mutex; 
void *atualizar(void *dados) {
    dados_threads *info = (dados_threads *) dados;
    int consultorio;
    char nome[32];

    FILE *fp = fopen(info->arquivo, "r"); 
    if (!fp) { perror(info->arquivo); return NULL; }

    
    while (fscanf(fp, "%31s %d", nome, &consultorio) == 2) {

        
        pthread_mutex_lock(&mutex[consultorio - 1]);

        
        
        printf("\033[%d;1H", consultorio);

        
        printf("\033[38;5;%d;48;5;%dm Paciente %-20s Consultório %d\033[0m",
               texto[consultorio - 1],
               cor[consultorio - 1],
               nome,
               consultorio);
        fflush(stdout);         
        sleep(2);               
        pthread_mutex_unlock(&mutex[consultorio - 1]);
    }
    fclose(fp);
    return NULL;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <tabela_inicial> [arquivos de atualização...]\n", argv[0]);
        return 1;
    }

    int consultorios;
    char nome[32];

    FILE *fp = fopen(argv[1], "r");
    if (!fp) { perror(argv[1]); return 1; }

    fscanf(fp, "%d", &consultorios);

    cor   = malloc(consultorios * sizeof(int));
    texto = malloc(consultorios * sizeof(int));

    printf("\033[2J\033[1;1H"); 
    
    for (int i = 0; i < consultorios; i++) {
        fscanf(fp, "%31s", nome);      
        cor[i] = (i * 30) % 256;        
        if (cor[i] < 128) texto[i] = 15;
        else              texto[i] = 0;

        printf("\033[38;5;%d;48;5;%dm Paciente %-20s Consultório %d\033[0m\n",
               texto[i], cor[i], nome, i + 1);
    }
    fclose(fp);

    
    mutex = malloc(consultorios * sizeof(pthread_mutex_t));
    for (int i = 0; i < consultorios; i++) {
        pthread_mutex_init(&mutex[i], NULL);
    }

    
    int n_threads = argc - 2;
    pthread_t threads[n_threads];
    dados_threads dados[n_threads];

    
    for (int t = 0; t < n_threads; t++) {
        strcpy(dados[t].arquivo, argv[t + 2]);
        pthread_create(&threads[t], NULL, atualizar, &dados[t]);
    }

    for (int a = 0; a < n_threads; a++) {
        pthread_join(threads[a], NULL);
    }

    
    for (int i = 0; i < consultorios; i++) {
        pthread_mutex_destroy(&mutex[i]);
    }

    free(cor);
    free(texto);
    free(mutex);

    printf("\033[%d;1H\n", consultorios + 1); 
    return 0;
}

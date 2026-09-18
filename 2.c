/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define MAX_pessoas 64
#define NUM_arquivos 3   
#define NUM_Linhas 4  
#define  COOLDOWN_SEC 2  
#define  MAX_tentativas 200

typedef struct{
    char nome[MAX_pessoas];
    int sala;
}Ficha;

typedef struct{
    pthread_mutex_t mutx;
    int ocupado;//avisa se o "banheiro" ta ocupado.
}fila;

static Ficha files[NUM_arquivos][NUM_Linhas];
static fila  lines[NUM_Linhas];
static pthread_mutex_t print_mutx = PTHREAD_MUTEX_INITIALIZER;

void criar_arquivos(){
    int t=0;
    int i=0;
    for (t=0;t<NUM_arquivos;t++){
    char fichanome[MAX_pessoas];
    snprintf(fichanome, sizeof fichanome, "consultorio_%d.txt", t + 1);
    FILE *fp = fopen(fichanome,"w"); //sdds CE
    for (i = 0; i < NUM_Linhas; i++){
            fprintf(fp, "Paciente%d_%d %d\n", t + 1, i, t + 1);
        }fclose(fp);
    }
}
void carrega_arquivos(){
    int x=0;
    for (x=0;x<NUM_arquivos;x++){
        char fichanome[64];
        snprintf(fichanome, sizeof fichanome, "consultorio_%d.txt", x + 1);
        FILE *fp = fopen(fichanome,"r");
        for (int i = 0; i < NUM_Linhas; ++i) {
            if (fscanf(fp, "%63s %d",files[x][i].nome, &files[x][i].sala) != 2) {
                fprintf(stderr, "Formato errado em %s\n", fichanome);
                exit(EXIT_FAILURE);
            }
        }
        fclose(fp);
    }
}
void ui(){
    int u=0;
    printf("\033[2J\033[H");//comando de ANSI
    printf("chamadas");
    for (u=0;u<NUM_arquivos;u++){
        printf("| Linha %d | %-30s | Sala: %-3s |\n", u, "(aguardando)", "---");
    }
    printf("......\n");
    fflush(stdout);
    //return 0;
}
//void da thread
typedef struct 
{
    int file_id; //id da cada ficha
}threadargs;

int pega(int idx){
  pthread_mutex_lock(&lines[idx].mutx);
  if (lines[idx].ocupado){ //"tem alguem no banheiro"
    pthread_mutex_unlock(&lines[idx].mutx);
    return 0;
  }
  lines[idx].ocupado=1;
  pthread_mutex_unlock(&lines[idx].mutx);//se nao tinha alguem no banheiro,agr tem.
  return 1;
}

void solta(int idx){
    pthread_mutex_lock(&lines[idx].mutx);
    lines[idx].ocupado = 0 ; //sai do banheiro
    pthread_mutex_unlock(&lines[idx].mutx);
}

void operador(void *args){
    int i=0;
    threadargs *p1 = (threadargs*)args; //ponteiro para andar
    int ficha_id= p1->file_id;//aonde o ponteiro ta
    for(i=0;i<NUM_Linhas;i++){
        Ficha p = files[ficha_id][i];
        int alvo=-1;
        //round-robin
        for(int tentativas =0;tentativas< MAX_tentativas;tentativas++){
            if(alvo<-1){
                for (int k = 0; k < NUM_Linhas; k++)
                {
                    int idx = (i+k)%NUM_Linhas;
                    if(pega(idx)){
                        alvo=idx;
                        break;//ebaaa posso usar o break
                    }
                }
                if (alvo<-1) usleep(1000);
            }
        }
        if (alvo <0) continue;
        faz_linha(alvo,&p);
        sleep(COOLDOWN_SEC);
        solta(alvo);
    }

}

void faz_linha(int idx,const Ficha *p){
    pthread_mutex_lock(&print_mutx);
    printf("\033[%d;1H", idx + 2);
    printf("| Linha %d | %-30s | Sala: %-3d |", idx, p->nome, p->sala);
    fflush(stdout);
    pthread_mutex_unlock(&print_mutx);
}

int main()
{
   int i = 0;
   int f =0;
   criar_arquivos();
   carrega_arquivos();
   
   for (i=0;i<NUM_Linhas;i++){
    pthread_mutex_init(&lines[i].mutx,NULL);
    lines[i].ocupado=0; //banheiros liberados
   }
   ui();

   pthread_t th[NUM_arquivos];
   threadargs args[NUM_arquivos];
   for (f=0;f<NUM_arquivos;f++){
    args[f].file_id = f;
    pthread_create(&th[f],NULL,operador,&args[f]);
    pthread_join(&th[f],NULL);
   }

   printf("\033[%d;1H\n", NUM_Linhas + 3);
    for (int i = 0; i < NUM_Linhas; ++i)
        pthread_mutex_destroy(&lines[i].mutx);
    return 0;
}
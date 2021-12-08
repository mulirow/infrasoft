#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Definindo quantidade de threads
#define N 4
#define M 4

// Definindo tamanho do array a ser modificado
#define TAM_MAX 32

char array[TAM_MAX];

// Mutex e variável de condição para controle do array
pthread_mutex_t array_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t array_cond = PTHREAD_COND_INITIALIZER;

// Rotina das threads de escrita
void *write_routine(void *threadid){
    int ID = (*(int *)threadid);
    int pos_write;

    while(1){
        // Obtenção de posição aleatória do array para a thread escrever
        pos_write = rand() % TAM_MAX;

        // Lock da região crítica (array)
        pthread_mutex_lock(&array_mutex);
        array[pos_write] = 'w';
        printf("Thread de escrita %d escreveu na posicao %d.\n", ID, pos_write);
        pthread_mutex_unlock(&array_mutex);

        // Envio de sinal de fim da escrita para liberar todas as threads em espera
        pthread_cond_broadcast(&array_cond);

        // Delay para facilitar visualização no terminal
        sleep(1); 
    }
}

// Rotina das threads de leitura
void *read_routine(void *threadid){
    int ID = (*(int *)threadid);
    int pos_read;

    while(1){
        // Obtenção de posição aleatória do array para a thread ler
        pos_read = rand() % TAM_MAX;

        // Espera por liberação caso esteja ocorrendo escrita
        pthread_cond_wait(&array_cond, &array_mutex);
        printf("Thread de leitura %d leu a posicao %d -> %c\n", ID, pos_read, array[pos_read]);

        // Delay para facilitar visualização no terminal
        sleep(1);
    }
}

int main(){
    //Declaração de threads e arrays respectivos para auxiliar identificação
    pthread_t thread_write[M];
    pthread_t thread_read[N];
    int write_id[M], read_id[N];
    
    int i, rc;

    // Inicialização do array
    for(i = 0; i < TAM_MAX; i++){
        array[i] = 'u';
    }

    // Criação de threads de escrita
    for(i = 0; i < M; i++){
        write_id[i] = i;
        rc = pthread_create(&thread_write[i], NULL, write_routine, &write_id[i]);
        if(rc){
            printf("Falhou na criacao da thread de escrita %d.\n", i);
            exit(1);
        }
    }

    // Criação de threads de leitura
    for(i = 0; i < N; i++){
        read_id[i] = i;
        rc = pthread_create(&thread_read[i], NULL, read_routine, &read_id[i]);
        if(rc){
            printf("Falhou na criacao da thread de leitura %d.\n", i);
            exit(1);
        }
    }

    // Garantia para não ocorrer encerramento da main
    for(i = 0; i < N; i++){
        pthread_join(thread_read[i], NULL);
    }
    
    pthread_exit(NULL);
}
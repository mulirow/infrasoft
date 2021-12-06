#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define N 4
#define M 4
#define TAM_MAX 32

char array[TAM_MAX];
pthread_mutex_t array_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t array_cond = PTHREAD_COND_INITIALIZER;

void *write_routine(void *threadid){
    int ID = (*(int *)threadid);
    int pos_write;
    while(1){
        pos_write = rand()%TAM_MAX;
        pthread_mutex_lock(&array_mutex);
        array[pos_write] = 'w';
        printf("Thread de escrita %d escreveu na posicao %d.\n", ID, pos_write);
        pthread_mutex_unlock(&array_mutex);
        pthread_cond_broadcast(&array_cond);
        sleep(1);
    }
}

void *read_routine(void *threadid){
    int ID = (*(int *)threadid);
    int pos_read;
    while(1){
        pos_read = rand()%TAM_MAX;
        pthread_cond_wait(&array_cond, &array_mutex);
        printf("Thread de leitura %d leu a posicao %d -> %c\n", ID, pos_read, array[pos_read]);
        sleep(1);
    }
}

int main(){
    pthread_t thread_write[M];
    pthread_t thread_read[N];
    int write_id[M], read_id[N];
    
    int i, rc;

    for(i = 0; i < TAM_MAX; i++){
        array[i] = 'u';
    }

    for(i = 0; i < M; i++){
        write_id[i] = i;
        rc = pthread_create(&thread_write[i], NULL, write_routine, &write_id[i]);
        if(rc){
            printf("Falhou na criacao da thread de escrita %d.\n", i);
            exit(1);
        }
    }

    for(i = 0; i < N; i++){
        read_id[i] = i;
        rc = pthread_create(&thread_read[i], NULL, read_routine, &read_id[i]);
        if(rc){
            printf("Falhou na criacao da thread de leitura %d.\n", i);
            exit(1);
        }
    }

    for(i = 0; i < N; i++){
        pthread_join(thread_read[i], NULL);
    }
    
    pthread_exit(NULL);
}
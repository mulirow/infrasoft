#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 4
#define M 4
#define TAM_MAX 1024

char array[TAM_MAX];
int pos_write = 0, pos_read = 0;

void *write(void *threadid){
    while(1){
        pos_write = randgen()%1024;
        
    }
}

void *read(void *threadid){
    while(1){
        pos_read = randgen()%1024;
        
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
        rc = pthread_create(&thread_write[i], NULL, write, &write_id[i]);
        if(rc){
            printf("Falhou na criacao da thread de escrita %d.\n", i);
            exit(1);
        }
    }

    for(i = 0; i < N; i++){
        read_id[i] = i;
        rc = pthread_create(&thread_read[i], NULL, read, &read_id[i]);
        if(rc){
            printf("Falhou na criacao da thread de leitura %d.\n", i);
            exit(1);
        }
    }

    for(i = 0; i < N; i++){
        pthread_join(read_id[i], NULL);
    }


    pthread_exit(NULL);
}
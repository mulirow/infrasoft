#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int *array, contador = 0;
int N, T;

void *Erastosthenes(void *threadid);

int main(){
    int i;
    
    scanf("%d %d", &N, &T);
    pthread_t Threads[T];
    int ThreadsID[T], rc;
    array = (int *) malloc(N * sizeof(int));

    for(i = 0; i < N; i++){
        if(i == 0 || i == 1){
            array[i] = 0;
        }
        else{
            array[i] = 1;
        }
    }

    for(i = 0; i < T; i++){
        ThreadsID[i] = i;
        rc = pthread_create(&Threads[i], NULL, Erastosthenes, &ThreadsID[i]);
        printf("Criando Thread: %d\n", i);
        if(rc){
            printf("Falhou na criação da Thread %d\n", i);
            exit(-1);
        }
    }

    free(array);
    pthread_exit(NULL);
}

//Proibido ser feliz
//Tem questão 9?
// ss
// onde???
// dps da 8
// '-' vi agora
// :)
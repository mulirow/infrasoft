#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS 10
#define VALOR 1000000

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
int contador = 0, flag=0;

void *inc (void *threadid){
    int ID = *((int *)threadid);

    while(contador < VALOR){
        pthread_mutex_lock(&mymutex);
        if(contador < VALOR){
            printf("A thread %d esta incrementando (VALOR ATUAL: %d).\n", ID, contador); 
            contador++; 
        }
        if(contador == VALOR && flag == 0){
            printf("A Thread %d foi a milionesima incrementadora!\n", ID);
            flag = 1;
        }
        pthread_mutex_unlock(&mymutex);
    }
    
    pthread_exit(NULL);
}

int main (){
    pthread_t Thread[NUM_THREADS];
    int ThreadID[NUM_THREADS];
    int i, rc;

    //Inicializando os ID's
    for(i=0; i<NUM_THREADS; i++){
        ThreadID[i]=i;
        printf("Criando Thread: %d", i);
        rc = pthread_create(&Thread[i], NULL, inc, &ThreadID[i]);
        if(rc){
            printf("Fomos de Base na %d Thread\n", i);
            exit(1);
        }
        //pthread_join(Thread[i], NULL);
    }

    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(Thread[i], NULL);
    }
    printf("Valor final: %d\n", contador);
    pthread_exit(NULL);
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int *array,*guardadordeprimos, *kc;
int N, T, prime = 0;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

void *Eratosthenes(void *threadid){
    int ID = (*(int *)threadid);
    int i;

    while(prime * prime <= N){
        //Escolhendo o proximo elemento
        pthread_mutex_lock(&mymutex);
        for(i = prime + 1; i <= N; i++){
            if(array[i] == 1){
                prime = i;
                break;
            }
        }
        guardadordeprimos[ID]=prime;
        pthread_mutex_unlock(&mymutex);

        printf("%d encontrado pela thread %d\n", prime, ID);

        //Eliminação de não-primos
        for(kc[ID] = guardadordeprimos[ID] + guardadordeprimos[ID]; kc[ID] < N; kc[ID] += guardadordeprimos[ID]){
            array[kc[ID]] = 0;
        }
    }

    pthread_exit(NULL);
}

int main(){
    int i;

    scanf("%d %d", &N, &T);
    pthread_t Threads[T];
    int ThreadsID[T], rc;
    array = (int *) malloc(N * sizeof(int));
    guardadordeprimos = (int *) malloc(T * sizeof(int));
    kc = (int *) malloc(T * sizeof(int));

    for(i = 0; i < N; i++){
        array[i] = 1;
    }

    array[0] = 0;
    array[1] = 0;

    for(i = 0; i < T; i++){
        ThreadsID[i] = i;
        rc = pthread_create(&Threads[i], NULL, Eratosthenes, &ThreadsID[i]);
        printf("Criando Thread: %d\n", i);
        if(rc){
            printf("Falhou na criação da Thread %d\n", i);
            exit(1);
        }
    }

    for(i = 0; i < T; i++){
        pthread_join(Threads[i], NULL);
    }

    int count = 0;
    printf("Primos encontrados: ");
    for(i = 0; i < N; i++){
        if(array[i] == 1){
            count++;
        }
    }
    printf("%d\n", count);

    free(array);
    free(guardadordeprimos);
    free(kc);
    pthread_exit(NULL);
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int *array;
int N, T, prime = 0;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

void *Eratosthenes(void *threadid){
    int ID = (*(int *)threadid);
    int i;

    // Limite de procura do crivo
    while(prime * prime <= N){
        // Escolhendo o proximo primo
        // Regiões críticas: prime e *array
        pthread_mutex_lock(&mymutex);
        for(i = prime + 1; i <= N; i++){
            if(array[i] == 1){
                prime = i;
                break;
            }
        }
        int currPrime = prime;
        pthread_mutex_unlock(&mymutex);

        //Eliminação de não-primos
        for(int k = 2 * currPrime; k < N; k += currPrime){
            array[k] = 0;
        }
    }

    pthread_exit(NULL);
}

int main(){
    int i;
    printf("Insira a quantidade de numeros do crivo (0 incluso) e a quantidade de threads: ");
    scanf("%d %d", &N, &T);
    pthread_t Threads[T];
    int ThreadsID[T], rc;
    array = (int *) malloc(N * sizeof(int));

    // Inicialização
    for(i = 0; i < N; i++){
        array[i] = 1;
    }
    array[0] = 0;
    array[1] = 0;

    for(i = 0; i < T; i++){
        ThreadsID[i] = i;
        rc = pthread_create(&Threads[i], NULL, Eratosthenes, &ThreadsID[i]);
        // printf("Criando Thread: %d\n", i);
        if(rc){
            printf("Falhou na criação da Thread %d\n", i);
            exit(1);
        }
    }

    for(i = 0; i < T; i++){
        pthread_join(Threads[i], NULL);
    }

    int primeCount = 0;
    printf("Primos encontrados: ");
    for(i = 2; i < N; i++){
        if(array[i] == 1){
            printf("%d ", i);
            primeCount++;
        }
    }
    printf("\nTotal de primos: %d\n", primeCount);

    free(array);
    pthread_exit(NULL);
}
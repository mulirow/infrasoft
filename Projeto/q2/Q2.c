#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int flagID = 1;
int *arrayPartition, *vetor;

void *partCheck(void *threadid){
    int ID = (*(int *)threadid);
    printf("Entrando na Thread %d (VALOR: %d)\n", ID, flagID);

    // Checa ordenação
    for(int i = arrayPartition[ID]; i < arrayPartition[ID+1]; i++){
        if(vetor[i+1] < vetor[i]){
            flagID = 0;
        }
    }
    printf("Saindo da Thread %d (VALOR: %d)\n", ID, flagID);
}

int main(){
    int arraySize, N;

    printf("Digite o tamanho da array: ");
    scanf("%d", &arraySize);
    vetor = (int *) calloc(arraySize, sizeof(int));

    printf("Digite o Array:\n");
    for(int i = 0; i < arraySize; i++){
        scanf("%d", &vetor[i]);
    }

    printf("Digite o numero de threads: ");
    scanf("%d", &N);

    if(N > arraySize){
        printf("Existem mais threads que o tamanho da array.\n");
        N = arraySize;
        printf("O número de threads foi atualizado para o tamanho do array.\n");
    }
    
    arrayPartition = (int *) calloc(N + 1, sizeof(int));
    pthread_t threads[N];
    int threadID[N], rc;

    int defaultSize = arraySize / N, extra = arraySize % N;

    // Divide as posições de cada thread, dividindo o resto o mais igualmente possível
    for(int i = 1; i <= N; i++){
        if(extra > 0){
            arrayPartition[i] = arrayPartition[i-1] + defaultSize + 1;
            extra--;
        }
        else{
            arrayPartition[i] = arrayPartition[i-1] + defaultSize;
        }
    }
    // Impedir invasão de memória na última posição
    arrayPartition[N]--;

    //Criando as threads
    for(int i = 0; i < N; i++){
        threadID[i] = i;
        rc = pthread_create(&threads[i], NULL, partCheck, &threadID[i]);
        printf("Criando Thread: %d\n", i);
        if(rc){ 
            printf("Fomos de Base na %d Thread\n", i);
            exit(1);
        }   
    }
    
    for(int i = 0; i < N; i++){
        pthread_join(threads[i], NULL);
    }
    
    if(flagID){
        printf("O vetor esta ordenado!\n");
    }else{
        printf("O vetor nao esta ordenado!\n");
    }

    free(arrayPartition);
    free(vetor);
    pthread_exit(NULL);
}
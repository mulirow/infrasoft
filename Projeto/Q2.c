#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int FlagID=1, *ArrayPartition, *Vetor;

void *partCheck(void *threadid){
    int ID = *((int *)threadid);
    printf("Entrando na Thread %d (VALOR: %d)\n", ID, FlagID);
    for(int i = ArrayPartition[ID]; i < ArrayPartition[ID+1]; i++){
        if(Vetor[i+1] < Vetor[i]){
            FlagID=0;
        }
    }  
    printf("Saindo da Thread %d (VALOR: %d)\n", ID, FlagID);
}

int main(){
    int ArraySize, N;
    printf("Digite o tamanho da array: ");
    scanf("%d", &ArraySize);
    Vetor = (int *) calloc(ArraySize, sizeof(int));

    printf("Digite o Array:\n");
    for(int i=0; i<ArraySize; i++){
        scanf("%d", &Vetor[i]);
    }

    printf("Digite o numero de threads: ");
    scanf("%d", &N);

    if(N>ArraySize){
        printf("Tem mais Threads que o tamanho da array.\n");
        N=ArraySize;
        printf("O número de threads foi atualizado para o tamanho do array.\n");
    }
    
    ArrayPartition = (int *) calloc(N + 1, sizeof(int));
    pthread_t Threads[N];
    int ThreadsID[N], rc;

    int defaultSize = ArraySize / N, extra = ArraySize % N;

    for(int i = 1; i <= N; i++){
        if(extra > 0){
            ArrayPartition[i] = ArrayPartition[i-1] + defaultSize + 1;
            extra--;
        }
        else{
            ArrayPartition[i] = ArrayPartition[i-1] + defaultSize;
        }
    }
    // Impedir invasao de memoria na ultima particao
    ArrayPartition[N]--;

    //Criando as Threads
    for(int i = 0; i < N; i++){
        ThreadsID[i] = i;
        rc = pthread_create(&Threads[i], NULL, partCheck, &ThreadsID[i]);
        printf("Criando Thread: %d\n", i);
        if(rc){ 
            printf("Fomos de Base na %d Thread\n", i);
            exit(-1);
        }   
    }
    
    for(int i=0; i<N;i++){
        pthread_join(Threads[i],NULL);
    }
    
    if(FlagID){
        printf("O vetor esta ordenado!\n");
    }else{
        printf("O vetor nao esta ordenado!\n");
    }

    free(ArrayPartition);
    free(Vetor);
    pthread_exit(NULL);
}
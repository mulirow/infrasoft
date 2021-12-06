#include <bits/stdc++.h>
#include <pthread.h>
#include <iostream>
using namespace std;

typedef struct{
    int baseMaior;
    int baseMenor;
    int altura;
}Trapezio;

int ID=0, N, threadsOnline=0;
float *bufferAns;
queue<pair<pair<int,Trapezio>,int>> FilaInst; //{{Instrucao,Parâmetro},ID}

void *calculaArea(Trapezio parametros){
    float area;

    area = (parametros.baseMaior+parametros.baseMenor)*parametros.altura;
    area /= 2;
}
void *calculaPerimetroBases(Trapezio parametros){
    float perimetro;

    perimetro = parametros.baseMaior+parametros.baseMenor;
}

void agendarExecucao(int instrucao, Trapezio parametros){
    FilaInst.push({{instrucao,parametros},ID});
    printf("Instrucao #%d: Registrada!", ID);
    ID++;
}

void *DepartamentoDeDespache(void *threadid){
    int RC;
    pthread_t Exec_Threads[N];
    while(1){
        if(FilaInst.empty()){
            //I slip
        }else{
            //I wake
        }
        while(!(FilaInst.empty())){
            //Mutex Lock
            pair<pair<int,Trapezio>,int> bufferFila = FilaInst.front();
            if(threadsOnline<N){
                if(bufferFila.first.first == 1){
                    //criar a thread para area
                    
                    threadsOnline++;
                }else if(bufferFila.first.first == 2){
                    //cria a thread para perimetro
                    
                    threadsOnline++;
                }         
            }
            //Mutex Unlock
        }
    }
}
 

void pegarResultadoExecucao(int id){
    //Checar se a resposta realmente chegou
    printf("Instrucao #%d: %f.\n", id, bufferAns[id]);
}


int main(){
    int instrucao,rc, i;
    Trapezio parametros;
    pthread_t threadDespachante;
    bufferAns = (float *) calloc(1,sizeof(float));

    //Recebendo o número de Threads
    printf("Digite o numero de Threads disponiveis: ");
    scanf("%d", &N);

    //Criando a Thread Despachante
    rc = pthread_create(&threadDespachante, NULL, DepartamentoDeDespache, NULL);
    if(rc){
        printf("Ocorreu um erro na criação da Thread Despachante.\n");
        exit(1);
    }
    
    //Recebendo Instruções
    printf("Digite o numero da instrucao: ");
    while(scanf("%d", &instrucao)!=EOF){
        if(instrucao == 0){
            printf("Digite o ID da instrucao: ");
            scanf("%d", &instrucao);
            pegarResultadoExecucao(instrucao);
        }else{
            printf("Valor da base maior: ");
            scanf("%d", &parametros.baseMaior);
            printf("Valor da base menor: ");
            scanf("%d", &parametros.baseMenor);
            printf("Valor da altura: ");
            scanf("%d", &parametros.altura);
            agendarExecucao(instrucao, parametros);
        }
    }

    free(bufferAns); 
    pthread_exit(NULL);
}

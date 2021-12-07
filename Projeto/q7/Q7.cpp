#include <bits/stdc++.h>
#include <pthread.h>
#include <vector>
#include <iostream>
#define NUM_USERS 4
using namespace std;

typedef struct{
    int baseMaior;
    int baseMenor;
    int altura;
}Trapezio;

typedef struct{
    float resultado;
    int ID;
}Resultado;

int ID=0, N, threadsOnline=0;
vector<Resultado> bufferSaida;
queue<pair<pair<int,Trapezio>,int>> filaInst; //{{Instrucao,Parâmetro},ID}

pthread_mutex_t mutex_fila = PTHREAD_MUTEX_INITIALIZER;

void *calculaArea(void* arg){
    Trapezio *parametros = (Trapezio *) arg;
    float area;

    area = (parametros->baseMaior+parametros->baseMenor)*parametros->altura;
    area /= 2;
    //TO DO: Devolver o resultado
}
void *calculaPerimetroBases(void* arg){
    Trapezio *parametros = (Trapezio *) arg;
    float perimetro;

    perimetro = parametros->baseMaior+parametros->baseMenor;
    //TO DO: Devolver o resultado
}

void *acaoUsuario(void *threadid){
    int action, swap, instr;
    float answer;
    vector<int> IDs;
    Trapezio trapezio;

    while(1){
        action = rand()%2;
        trapezio.baseMaior = rand()%100;
        trapezio.baseMenor = rand()%100;
        trapezio.altura = rand()%100;
        
        if(trapezio.baseMenor > trapezio.baseMaior){
            swap = trapezio.baseMenor;
            trapezio.baseMenor = trapezio.baseMaior;
            trapezio.baseMaior = swap;
        }
        
        
        if(action == 0){
            instr = rand()%2;
            pthread_mutex_lock(&mutex_fila);
            IDs.push_back(agendarExecucao(instr,trapezio));
            pthread_mutex_unlock(&mutex_fila);
        }
        if(action == 1 && (!IDs.empty())){
            ID = IDs[rand()%(IDs.size())];
            answer = pegarResultadoExecucao(ID);
        }
    } 
}

// Coloca as instruções a serem executadas no buffer
int agendarExecucao(int instrucao, Trapezio parametros){
    auto filaElem = make_pair(make_pair(instrucao,parametros),ID);
    filaInst.push(filaElem);
    printf("Instrucao #%d: Registrada!", ID);
    ID++;
    return ID;
}

// Rotina da thread de despacho
void *DepartamentoDeDespache(void *threadid){
    int RC;
    pair<pair<int,Trapezio>,int> recebeFilaInst;
    pthread_t Exec_Threads[N];
    while(1){
        while(filaInst.empty()){
            //slip
        }
        //real shit 
        auto bufferFila = filaInst.front();
        if(threadsOnline<N){
            //Recebe FIla.front() 
            //POP
            //Colocamos ID no bufferSaida com resposta = -1
            //Puxamos enviamos para uma Thread
            
            //MUTEX
            recebeFilaInst = filaInst.front();
            filaInst.pop();
            bufferSaida.push_back({-1,recebeFilaInst.second});
            //MUTEX

        }
        
    }
}
 
// Obtem resultados que estejam disponiveis, bloqueando caso contrario
float pegarResultadoExecucao(int id){
    float resp;
    for(auto i = 0;i<bufferSaida.size();i++){
        if(id==bufferSaida[i].ID){
            if(bufferSaida[i].resultado==-1){
                //espera
                resp = bufferSaida[i].resultado;
                bufferSaida.erase(bufferSaida.begin()+i); //(MUTEX)
                return resp;
            }else{
                resp = bufferSaida[i].resultado;
                bufferSaida.erase(bufferSaida.begin()+i); //(MUTEX)
                return bufferSaida[i].resultado;
            }
        }
    }
}


int main(){
    int instrucao,rc, i;
    Trapezio parametros;
    pthread_t threadUsuarios[NUM_USERS];
    pthread_t threadDespachante;

    //Recebendo o número de Threads
    printf("Digite o numero de Threads disponiveis: ");
    scanf("%d", &N);

    //Criando threads de usuarios
    for(int i=0; i<NUM_USERS; i++){
        rc = pthread_create(&threadUsuarios[i], NULL, acaoUsuario, NULL);
    }

    //Criando a Thread Despachante
    rc = pthread_create(&threadDespachante, NULL, DepartamentoDeDespache, NULL);
    if(rc){
        printf("Ocorreu um erro na criação da Thread Despachante.\n");
        exit(1);
    }
    
    pthread_exit(NULL);
}

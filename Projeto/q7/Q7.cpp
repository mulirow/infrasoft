#include <bits/stdc++.h>
#include <pthread.h>
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
pthread_mutex_t mutex_saida = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waitBuffer = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitAns = PTHREAD_COND_INITIALIZER;

void *calculaArea(void* arg){
    pair<pair<int,Trapezio>,int> *instInfo = (pair<pair<int,Trapezio>,int> *) arg;
    int area;
    Trapezio parametros = instInfo->first.second;
    int id = instInfo->second;

    area = (parametros.baseMaior+parametros.baseMenor)*parametros.altura;
    area /= 2;
    pthread_mutex_lock(&mutex_saida);
    for(int i=0;i<bufferSaida.size();i++){
        if(id == bufferSaida[i].ID){
            bufferSaida[i].resultado = area; //(MUTEX)
            pthread_cond_broadcast(&waitAns);
            threadsOnline--;
        }
    }
    pthread_mutex_unlock(&mutex_saida);
    pthread_exit(NULL);
}
void *calculaPerimetroBases(void* arg){
    pair<pair<int,Trapezio>,int> *instInfo = (pair<pair<int,Trapezio>,int> *) arg;
    int perimetro;
    Trapezio parametros = instInfo->first.second;
    int id = instInfo->second;
    perimetro = parametros.baseMaior+parametros.baseMenor;

    pthread_mutex_lock(&mutex_saida);
    for(int i=0;i<bufferSaida.size();i++){
        if(id == bufferSaida[i].ID){
            bufferSaida[i].resultado = perimetro; //(MUTEX)
            pthread_cond_broadcast(&waitAns);
            threadsOnline--;
        }
    }
    pthread_mutex_unlock(&mutex_saida);
    pthread_exit(NULL);
}

// Coloca as instruções a serem executadas no buffer
int agendarExecucao(int instrucao, Trapezio parametros){
    auto filaElem = make_pair(make_pair(instrucao,parametros),ID);
    filaInst.push(filaElem);
    if(filaInst.size()==1){
        pthread_cond_signal(&waitBuffer);
    }
    printf("Instrucao #%d: Registrada\n", ID);
    ID++;
    return ID;
}

// Obtem resultados que estejam disponiveis, bloqueando caso contrario
float pegarResultadoExecucao(int id){
    float resp;
    pthread_mutex_lock(&mutex_saida);
    for(int i = 0;i<bufferSaida.size();i++){
        if(id == bufferSaida[i].ID){
            if(bufferSaida[i].resultado == -1){
                while(bufferSaida[i].resultado == -1){
                    //I SLIP
                    pthread_cond_wait(&waitAns,&mutex_saida);
                }
                resp = bufferSaida[i].resultado;
                bufferSaida.erase(bufferSaida.begin()+i); 
                pthread_mutex_unlock(&mutex_saida);
                return resp;
            }else{
                resp = bufferSaida[i].resultado;
                bufferSaida.erase(bufferSaida.begin()+i); 
                pthread_mutex_unlock(&mutex_saida);
                return resp;
            }
        }
    }
    pthread_mutex_unlock(&mutex_saida);
    printf("O ID nao foi encontrado.\n");
    return -1;
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
            instr = IDs[rand()%(IDs.size())];
            answer = pegarResultadoExecucao(instr);
            if(answer!=-1){
                printf("A resposta da Tread de ID#%d eh: %2.f\n", instr,answer);
            }
            
        }
    } 

    pthread_exit(NULL);
}


// Rotina da thread de despacho
void *DepartamentoDeDespache(void *threadid){
    int RC;
    pair<pair<int,Trapezio>,int> recebeFilaInst;
    pthread_t Exec_Threads[N];
    while(1){
        while(filaInst.empty()){
            //SLIP
            pthread_cond_wait(&waitBuffer,&mutex_fila);
        }
        //real shit 
        if(threadsOnline<N){         
            //LOCK
            pthread_mutex_lock(&mutex_fila);
            recebeFilaInst = filaInst.front();
            filaInst.pop();
            bufferSaida.push_back({-1,recebeFilaInst.second});
            //Criando threads executoras (REGIÃO CRÍTICA?)
            if(recebeFilaInst.first.first==1){
                threadsOnline++;
                if(pthread_create(&Exec_Threads[threadsOnline],NULL,calculaArea, &recebeFilaInst)){
                    printf("Erro na criacao da Thread executora #%d (INST#1)\n", threadsOnline);
                    exit(1);
                }
            }else if(recebeFilaInst.first.first==2){
                threadsOnline++;
                if(pthread_create(&Exec_Threads[threadsOnline],NULL,calculaPerimetroBases, &recebeFilaInst)){
                    printf("Erro na criacao da Thread executora #%d (ISNT#2)\n", threadsOnline);
                    exit(1);
                }
            }else{
                printf("Essa instrucao nao esta disponivel.\n");
            }
            pthread_mutex_unlock(&mutex_fila);
            //UNLOCK
        }else{
            //slip
            pthread_cond_wait(&waitBuffer,&mutex_fila);
        }
        
    }
    pthread_exit(NULL);
}
 


int main(){
    int instrucao, i;
    Trapezio parametros;
    pthread_t threadUsuarios[NUM_USERS];
    pthread_t threadDespachante;

    //Recebendo o número de Threads
    printf("Digite o numero de Threads disponiveis: ");
    scanf("%d", &N);

    //Criando threads de usuarios
    for(int i=0; i<NUM_USERS; i++){
        printf("Criando Thread de usuario #%d\n", i);
        if(pthread_create(&threadUsuarios[i], NULL, acaoUsuario, NULL)){
            printf("Erro na criacao da Thread de usuario #%d", i);
            exit(1);
        }
    }

    //Criando a Thread Despachante
    printf("Criando Thread Despachante\n");
    if(pthread_create(&threadDespachante, NULL, DepartamentoDeDespache, NULL)){
        printf("Ocorreu um erro na criação da Thread Despachante.\n");
        exit(1);
    }
    
    pthread_exit(NULL);
}

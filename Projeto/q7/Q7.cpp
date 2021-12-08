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
pthread_mutex_t mutex_busca = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waitBuffer = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitAns = PTHREAD_COND_INITIALIZER;

//Função para calcular Area
void *calculaArea(void* arg){
    pair<pair<int,Trapezio>,int> *instInfo = (pair<pair<int,Trapezio>,int> *) arg;
    int area;
    Trapezio parametros = instInfo->first.second;
    int id = instInfo->second;

    area = (parametros.baseMaior+parametros.baseMenor)*parametros.altura;
    area /= 2;

    //Procura pelo vetor de saída se já existe uma instrução registrada com o ID requisitado (Caso ache, ela atualiza o vetor com a resposta e emite um sinal)
    pthread_mutex_lock(&mutex_saida);
    for(int i=0;i<bufferSaida.size();i++){
        if(id == bufferSaida[i].ID){
            bufferSaida[i].resultado = area; //(exige MUTEX)
            pthread_cond_broadcast(&waitAns);
            threadsOnline--;
        }
    }
    pthread_mutex_unlock(&mutex_saida);
    pthread_exit(NULL);
}

//Função para calcular Perimetro
void *calculaPerimetroBases(void* arg){
    pair<pair<int,Trapezio>,int> *instInfo = (pair<pair<int,Trapezio>,int> *) arg;
    int perimetro;
    Trapezio parametros = instInfo->first.second;
    int id = instInfo->second;
    perimetro = parametros.baseMaior+parametros.baseMenor;

    //Procura pelo vetor de saída se já existe uma instrução registrada com o ID requisitado (Caso ache, ela atualiza o vetor com a resposta e emite um sinal)
    pthread_mutex_lock(&mutex_saida);
    for(int i=0;i<bufferSaida.size();i++){
        if(id == bufferSaida[i].ID){
            bufferSaida[i].resultado = perimetro; //(exige MUTEX)
            pthread_cond_broadcast(&waitAns);
            threadsOnline--;
        }
    }
    pthread_mutex_unlock(&mutex_saida);
    pthread_exit(NULL);
}

// Coloca as instruções a serem executadas na fila e retorna o ID dessa instrução
int agendarExecucao(int instrucao, Trapezio parametros){
    auto filaElem = make_pair(make_pair(instrucao,parametros),ID);
    filaInst.push(filaElem);
    if(filaInst.size()==1){
        pthread_cond_signal(&waitBuffer);// Emite um sinal pra indicar que existe instruções na fila
    }
    printf("Instrucao #%d: Registrada\n", ID);
    ID++; // Gera um novo ID
    return ID;
}

// Procura pelos resultados que estejam disponiveis, bloqueando caso contrario
float pegarResultadoExecucao(int id){
    float resp;
    pthread_mutex_lock(&mutex_saida);
    for(int i = 0;i<bufferSaida.size();i++){
        if(id == bufferSaida[i].ID){
            if(bufferSaida[i].resultado == -1){ // Caso o resultado seja -1, significa que a resposta não foi computada ainda, então a thread dorme
                while(bufferSaida[i].resultado == -1){
                    pthread_cond_wait(&waitAns,&mutex_saida);
                }
                //A thread acorda, devolve o resultado e retira essa instrução da fila
                resp = bufferSaida[i].resultado;
                bufferSaida.erase(bufferSaida.begin()+i); 
                pthread_mutex_unlock(&mutex_saida);
                return resp;
            }else{
                //A thread devolve o resultado e retira essa instrução da fila
                resp = bufferSaida[i].resultado;
                bufferSaida.erase(bufferSaida.begin()+i); 
                pthread_mutex_unlock(&mutex_saida);
                return resp;
            }
        }
    }
    pthread_mutex_unlock(&mutex_busca);
    printf("O ID nao foi encontrado.\n");
    return -1;
}

//Função que gera e emite as instruções e parametros a serem agendados
void *acaoUsuario(void *threadid){
    int action, swap, instr;
    float answer;
    vector<int> IDs;//Um vetor para guardar os ID's atualmente registrados no sistema
    Trapezio trapezio;

    while(1){
        //Gera os parametros
        action = rand()%2;
        trapezio.baseMaior = rand()%100;
        trapezio.baseMenor = rand()%100;
        trapezio.altura = rand()%100;
        
        if(trapezio.baseMenor > trapezio.baseMaior){
            swap = trapezio.baseMenor;
            trapezio.baseMenor = trapezio.baseMaior;
            trapezio.baseMaior = swap;
        }

        //Registra
        if(action == 0){ 
            instr = rand()%2;
            pthread_mutex_lock(&mutex_fila);
            IDs.push_back(agendarExecucao(instr,trapezio));
            pthread_mutex_unlock(&mutex_fila);
        }

        //Busca a resposta
        if(action == 1 && (!IDs.empty())){ 
            int index = rand()%(IDs.size());//Pega um índice aleatório do vetor de IDs
            instr = IDs[index];
            answer = pegarResultadoExecucao(instr);
            if(answer!=-1){
                printf("A resposta da Tread de ID#%d eh: %2.f\n", instr,answer);
                IDs.erase(IDs.begin()+index);
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
        while(filaInst.empty()){ // Dorme enquanto não tem nada na fila
            pthread_cond_wait(&waitBuffer,&mutex_fila);
        }
        //Resgata as informações da fila, despachando uma thread executora para esses valores
        if(threadsOnline<N){         
            pthread_mutex_lock(&mutex_fila);
            recebeFilaInst = filaInst.front();
            filaInst.pop();
            bufferSaida.push_back({-1,recebeFilaInst.second});

            if(recebeFilaInst.first.first==0){ //Gera uma Thread com rotina para calcular Area
                threadsOnline++;
                if(pthread_create(&Exec_Threads[threadsOnline],NULL,calculaArea, &recebeFilaInst)){
                    printf("Erro na criacao da Thread executora #%d (INST#1)\n", threadsOnline);
                    exit(1);
                }
            }else if(recebeFilaInst.first.first==1){ //Gera uma Thread com rotina para calcular Perímetro
                threadsOnline++;
                if(pthread_create(&Exec_Threads[threadsOnline],NULL,calculaPerimetroBases, &recebeFilaInst)){
                    printf("Erro na criacao da Thread executora #%d (ISNT#2)\n", threadsOnline);
                    exit(1);
                }
            }else{
                printf("Essa instrucao nao esta disponivel.\n");
            }
            pthread_mutex_unlock(&mutex_fila);
        }else{
            pthread_cond_wait(&waitBuffer,&mutex_fila);//A thread dorme caso o limite de threads executoras tenha sido alcançado
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

    //Criando a Thread Despachante
    printf("Criando Thread Despachante\n");
    if(pthread_create(&threadDespachante, NULL, DepartamentoDeDespache, NULL)){
        printf("Ocorreu um erro na criação da Thread Despachante.\n");
        exit(1);
    }

    //Criando threads de usuarios
    for(int i=0; i<NUM_USERS; i++){
        printf("Criando Thread de usuario #%d\n", i);
        if(pthread_create(&threadUsuarios[i], NULL, acaoUsuario, NULL)){
            printf("Erro na criacao da Thread de usuario #%d", i);
            exit(1);
        }
    }

    pthread_exit(NULL);
}

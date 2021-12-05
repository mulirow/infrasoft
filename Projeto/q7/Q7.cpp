#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

typedef struct{
    int baseMaior;
    int baseMenor;
    int altura;
}Trapezio;

float calculaTrapezio(Trapezio parametros){
    float area;

    area = (parametros.baseMaior+parametros.baseMenor)*parametros.altura;
    area /= 2;

    return area;
}

int agendarExecução(float (*funk)(Trapezio), Trapezio parametros){
    //buffer.push([funk] {});
    /*
    Queue? -> Fila das exec. pendentes.                 
    agendarExecucao(Instrucao, structParâmetros) -> Coloca a requisição da instrução ;               
    ID -> Utilizado para buscar o resultado fornecido por "funexec"
    */

}


int ID=0;
queue<function<float()>> buffer;

int main(){
    /*

    Despachante -> pop na queue, gerencia as N threads. Caso a fila esteja vazia, dorm. Se não, acorda.
    Somente quando uma thread acaba, é chamada outra (join?).
    O resultado de uma funexec é armazenado temporariamente e depois associada ao ID (agendarExecucão, que será retornado.)

    pegarResultadoExecucao(ID)-> Usuário fica bloquado até que a execucao seja concluída (Resultado Retornado). 
    */

    int N;
    Trapezio Parametros;

    scanf("%d ",N);
    pthread_t Threads[N];
    int ThreadID[N];

    while(scanf("%d %d %d", &Parametros.baseMaior, &Parametros.baseMenor, &Parametros.altura)!=EOF){
        agendarExecução(&calculaTrapezio, Parametros);
    }
}

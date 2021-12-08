#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <pthread.h>
typedef std::pair<int, int> ii;

// Dimensões da matriz (precisa modificar para diferentes tamanho)
#define M 10
#define N 20

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t matrix_mutex[M][N];

// Queue para controlar as próximas posições analizadas
std::queue<ii> q;

// Controle do labirinto
int matrix[M][N];
bool visit[M][N];
ii parent[M][N];
ii* curr = NULL;

// Quantidade de threads do nível atual da BFS
int threadCount = 1;

void* isTherePath(void* threadid){ // BFS
    int ID = (*(int *)threadid);

    // Possibilidades de movimento (N, E, S, W)
    int distX[4] = {-1, 0, 1, 0};
    int distY[4] = {0, 1, 0, -1};

    for(int i = 0; i < 4; i++){
        int x = curr[ID].first + distX[i];
        int y = curr[ID].second + distY[i];

        if((x >= 0) && (x < M) && (y >= 0) && (y < N)){
            // Região crítica: **visit
            pthread_mutex_lock(&matrix_mutex[x][y]);
            if(!visit[x][y]){
                visit[x][y] = true;

                if(matrix[x][y] == 0){
                    // Região crítica: q
                    pthread_mutex_lock(&queue_mutex);
                    q.push({x, y});
                    parent[x][y] = {curr[ID].first, curr[ID].second};
                    threadCount++;
                    pthread_mutex_unlock(&queue_mutex);
                }
            }
            pthread_mutex_unlock(&matrix_mutex[x][y]);
        }
    }

    pthread_exit(NULL);
}

int main(){
    int matrixInput;
    int startX, startY, endX, endY, i, j;
    bool ansPath = false;
    std::ifstream fs;
    fs.open("input.txt");

    // Variáveis de threads
    pthread_t threads[M*N];
    int threadID[M*N];
    int rc;

    // Inicializa as matrizes globais e lê o labirinto a partir do arquivo
    for(i = 0; i < M; i++){
        for(j = 0; j < N; j++){
            visit[i][j] = false;
            parent[i][j] = {-1, -1};

            fs >> matrix[i][j];

            pthread_mutex_init(&matrix_mutex[i][j], NULL);
        }
    }

    std::cout << "Insira a linha e coluna do ponto de partida (0-" << M-1 << ", 0-" << N-1 << "): ";
    std::cin >> startX >> startY;
    
    // Coloca o ponto lido como origem
    q.push({startX, startY});
    visit[startX][startY] = true;
    parent[startX][startY] = {startX, startY};

    std::cout << "Insira a linha e coluna do ponto de saida(0-" << M-1 << ", 0-" << N-1 << "), diferente do ponto de partida: ";
    std::cin >> endX >> endY;
    
    while(!q.empty() && !ansPath){
        // Determina a quantidade de threads a serem usadas nessa iteração
        // de acordo com quantos push houveram na iteração passada
        int joinValue = threadCount;
        i = threadCount;
        threadCount = 0;

        curr = (ii*) realloc(curr, i * sizeof(ii));

        while(i > 0){
            threadID[i-1] = i-1;

            // Regiões críticas: curr e q
            pthread_mutex_lock(&queue_mutex);
            curr[i-1] = q.front();
            q.pop(); 
            pthread_mutex_unlock(&queue_mutex);

            // Checa se chegou no destino
            if(curr[i-1].first == endX && curr[i-1].second == endY){
                ansPath = true;
                break;
            }

            // printf("Criando Thread: %d\n", i-1);
            rc = pthread_create(&threads[i-1], NULL, isTherePath, &threadID[i - 1]);
            if(rc){
                printf("Fomos de Base na %da Thread\n", i-1);
                exit(1);
            }

            i--;
        }

        // Sincroniza as threads a cada nível da BFS
        for(i = 0; i < joinValue; i++){
            pthread_join(threads[i], NULL);
        }
    }

    i = endX;
    j = endY;

    if(ansPath == true){
        std::cout << "Existe caminho. Do ponto de saida ao de partida: \n";
        while(parent[i][j] != std::make_pair(i, j)){
            std::cout << "(" << i << ", " << j << ") -> ";
            int tempi = parent[i][j].first;
            j = parent[i][j].second;
            i = tempi;
        }
        std::cout << "(" << i << ", " << j << ")\n";
    }
    else{
        std::cout << "N existe caminho. :(\n";
    }

    for(i = 0; i < M; i++){
        for(j = 0; j < N; j++){
            pthread_mutex_destroy(&matrix_mutex[i][j]);
        }
    }

    fs.close();
    pthread_exit(NULL);
}
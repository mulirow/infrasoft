#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
typedef std::pair<int, int> ii;

// Tamanho do labirinto e n de threads
#define N 4
#define NUM_THREADS 4
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

// Pilha de globais no canto do quarto
std::queue<ii> q;
int matrix[N][N];
bool visit[N][N];
int distX[4] = {-1, 0, 1, 0};
int distY[4] = {0, 1, 0, -1};
int endX, endY;
int x, y;
ii* temp = NULL;

void* isTherePath(void* threadid){ // BFS
    int ID = (*(int *)threadid);
    for(int i = 0; i < 4; i++){
        //to-do: x e y como array de tamanho NUM_THREAD
        x = temp[i].first + distX[i];
        y = temp[i].second + distY[i];

        //to-do: transformar o mutex externo em matriz de mutex
        pthread_mutex_lock(&queue_mutex);
        if(x >= 0 && x < N && y >= 0 && y < N && !visit[x][y]){
            
            visit[x][y] = true;
            
            if(matrix[x][y] == 0){
                pthread_mutex_lock(&queue_mutex);
                q.push({x, y});
                pthread_mutex_unlock(&queue_mutex);
            }
        }
        pthread_mutex_unlock(&queue_mutex);
    }

    //return false;
    pthread_exit(NULL);
}

int main(){
    int startX, startY, i, j;

    // Inicializa a matriz de células visitadas e lê o labirinto
    std::cout << "Insira a matriz do labirinto:\n";
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            visit[i][j] = false;

            int value;
            std::cin >> value;
            matrix[i][j] = value;
        }
    }

    std::cout << "Insira as coordenadas x e y do ponto de partida:\n";
    std::cin >> startX >> startY;
    q.push({startX, startY});
    visit[startX][startY] = true;

    std::cout << "Insira as coordenadas x e y do ponto de saida:\n";
    std::cin >> endX >> endY;

    pthread_t threads[NUM_THREADS];
    int threadID[NUM_THREADS];
    int rc;

    bool ans = false;
    int threadCount = 1;
    int flag = 1;

    while(!q.empty() && flag == 1){

        i = threadCount;
        threadCount = 0;
        temp = (ii*) realloc(temp, i * sizeof(ii));

        while(i > 0){
            threadID[i] = i;

            pthread_mutex_lock(&queue_mutex);
            temp[i-1] = q.front();
            q.pop(); 
            pthread_mutex_unlock(&queue_mutex);

            // Checa se chegou no destino
            if(temp[i-1].first == endX && temp[i-1].second == endY){
                ans = true;
                flag = 0;
                break;
            }

            printf("Criando Thread: %d\n", i-1);
            rc = pthread_create(&threads[i-1], NULL, isTherePath, &threadID[i]);
            if(rc){
                printf("Fomos de Base na %da Thread\n", i-1);
                exit(1);
            }

            i--;
        }

        for(i = 0; i < NUM_THREADS;i++){
            pthread_join(threads[i], NULL);
        }
    }

    if(ans == true){
        std::cout << "Existe caminho\n";
    }
    else{
        std::cout << "N existe caminho\n";
    }

    pthread_exit(NULL);
}
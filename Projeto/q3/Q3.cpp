#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
typedef std::pair<int, int> ii;

#define M 7
#define N 4
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t matrix_mutex[M][N];

// Pilha de globais no canto do quarto
std::queue<ii> q;
ii parent[M][N];
int matrix[M][N];
bool visit[M][N];
int distX[4] = {-1, 0, 1, 0};
int distY[4] = {0, 1, 0, -1};
int *x = NULL, *y = NULL;
ii* temp = NULL;
int threadCount = 1;

void* isTherePath(void* threadid){ // BFS // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
    int ID = (*(int *)threadid); // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
    for(int i = 0; i < 4; i++){ // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
        x[ID] = temp[ID].first + distX[i]; // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
        y[ID] = temp[ID].second + distY[i]; // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
 // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
        if((x[ID] >= 0) && (x[ID] < M) && (y[ID] >= 0) && (y[ID] < N)){ // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
            pthread_mutex_lock(&matrix_mutex[x[ID]][y[ID]]); // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
            if(!visit[x[ID]][y[ID]]){ // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
                visit[x[ID]][y[ID]] = true; // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
 // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
                if(matrix[x[ID]][y[ID]] == 0){ // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
                    pthread_mutex_lock(&queue_mutex); // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
                    q.push({x[ID], y[ID]}); // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
                    parent[x[ID]][y[ID]] = {temp[ID].first, temp[ID].second}; // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
                    threadCount++; // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
                    pthread_mutex_unlock(&queue_mutex); // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
                } // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
            } // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
            pthread_mutex_unlock(&matrix_mutex[x[ID]][y[ID]]); // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih
        } // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
    } // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
 // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
    pthread_exit(NULL); // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
} // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 

int main(){
    int startX, startY, endX, endY, i, j;

    // Inicializa a matriz de células visitadas e lê o labirinto
    std::cout << "Insira a matriz do labirinto:\n";
    for(i = 0; i < M; i++){
        for(j = 0; j < N; j++){
            visit[i][j] = false;
 // fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih fvck ih 
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

    for(i = 0; i < M; i++){
        for(j = 0; j < N; j++){
            parent[i][j] = {-1, -1};
        }
    }
    parent[startX][startY] = {startX, startY};

    pthread_t threads[M*N];
    int threadID[M*N];
    int rc;

    bool ans = false;
    int flag = 1;

    for(i = 0; i < M; i++){
        for(j = 0; j < N; j++){
            pthread_mutex_init(&matrix_mutex[i][j], NULL);
        }
    }


    while(!q.empty() && flag == 1){
        int joinValue = threadCount;
        i = threadCount;
        threadCount = 0;
        temp = (ii*) realloc(temp, i * sizeof(ii));
        x = (int *) realloc(x, i * sizeof(int));
        y = (int *) realloc(y, i * sizeof(int));

        while(i > 0){
            threadID[i-1] = i-1;

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

            // printf("Criando Thread: %d\n", i-1);
            rc = pthread_create(&threads[i-1], NULL, isTherePath, &threadID[i - 1]);
            if(rc){
                printf("Fomos de Base na %da Thread\n", i-1);
                exit(1);
            }

            i--;
        }

        for(i = 0; i < joinValue; i++){
            pthread_join(threads[i], NULL);
        }
    }


    i = endX;
    j = endY;

    if(ans == true){
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
        std::cout << "N existe caminho\n";
    }

    for(i = 0; i < M; i++){
        for(j = 0; j < N; j++){
            pthread_mutex_destroy(&matrix_mutex[i][j]);
        }
    }

    free(x);
    free(y);
    pthread_exit(NULL);
}
#define _XOPEN_SOURCE 600
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <pthread.h>
typedef std::pair<int, int> ii;

#define NUM_THREADS 4
pthread_barrier_t barrier;
pthread_mutex_t join_mutex = PTHREAD_MUTEX_INITIALIZER;

// Controle da matriz
ii **root;
int **sz;
int **map;

// Dimensões, número de threads e resposta final
int x, y, T, numIslands = 0;

template <typename T>
T **allocMatrix(int height, int width){
    int i, j;
    T **matrix = (T **)malloc(height * sizeof(T *));
    if (matrix == NULL){
        printf("Failed to allocate memory.\n");
        exit(1);
    }
    for (i = 0; i < height; i++){
        matrix[i] = (T *)malloc(width * sizeof(T));
        if (matrix[i] == NULL){
            printf("Failed to allocate memory.\n");
            exit(1);
        }
    }

    return matrix;
}

template <typename T>
void freeMatrix(T **matrix, int height){
    int i;
    for (i = 0; i < height; i++){
        free(matrix[i]);
    }
    free(matrix);
}

ii find(ii a){
    if (root[a.first][a.second] == a){
        return a;
    }
    return find(root[a.first][a.second]);
}

void join(ii a, ii b){
    a = find(a);
    b = find(b);

    // Define quem vira root de quem e atualiza tamanhos
    if (a != b){
        if (sz[a.first][a.second] < sz[b.first][b.second]){
			root[a.first][a.second] = b;
        }
        else{
            root[b.first][b.second] = a;
        }

        int temp = sz[a.first][a.second];
		sz[a.first][a.second] += sz[b.first][b.second];
		sz[b.first][b.second] += temp;
        numIslands--;
    }
}

void *findIslands(void *threadid){
    int ID = (*(int *)threadid);

    // Checa linhas pares da matriz, threads alternando entre si para dividir o trabalho
    // Não há necesisdade de checar NO, N e NE, a junção desses com a linha atualmente checada
    // será feita na checagem de linhas ímpares
    for (int i = 2 * ID; i < x; i += 2 * T){
        for (int j = 0; j < y; j++){
            if (map[i][j] == 0){
                continue;
            }
            // E
            if ((j + 1 < y) && (map[i][j + 1] == 1)){
                // Região crítica: **root, **sz, numIslands (acessados dentro da função)
                pthread_mutex_lock(&join_mutex);
                join({i, j}, {i, j + 1});
                pthread_mutex_unlock(&join_mutex);
            }
            // SE
            if ((i + 1 < x) && (j + 1 < y) && (map[i + 1][j + 1] == 1)){
                pthread_mutex_lock(&join_mutex);
                join({i, j}, {i + 1, j + 1});
                pthread_mutex_unlock(&join_mutex);
            }
            // S
            if ((i + 1 < x) && (map[i + 1][j] == 1)){
                pthread_mutex_lock(&join_mutex);
                join({i, j}, {i + 1, j});
                pthread_mutex_unlock(&join_mutex);
            }
            // SO
            if ((i + 1 < x) && (j - 1 >= 0) && (map[i + 1][j - 1] == 1)){
                pthread_mutex_lock(&join_mutex);
                join({i, j}, {i + 1, j - 1});
                pthread_mutex_unlock(&join_mutex);
            }
            // O
            if ((j - 1 >= 0) && (map[i][j - 1] == 1)){
                pthread_mutex_lock(&join_mutex);
                join({i, j}, {i, j - 1});
                pthread_mutex_unlock(&join_mutex);
            }
        }
    }

    // Barreira para garantir que todos os joins entre linhas sejam feitos
    pthread_barrier_wait(&barrier);

    // Checagem das linhas ímpares
    for (int i = 2 * ID + 1; i < x; i += 2 * T){
        for (int j = 0; j < y; j++){
            if (map[i][j] == 0){
                continue;
            }
            // E
            if ((j + 1 < y) && (map[i][j + 1] == 1)){
                pthread_mutex_lock(&join_mutex);
                join({i, j}, {i, j + 1});
                pthread_mutex_unlock(&join_mutex);
            }
            // SE
            if ((i + 1 < x) && (j + 1 < y) && (map[i + 1][j + 1] == 1)){
                pthread_mutex_lock(&join_mutex);
                join({i, j}, {i + 1, j + 1});
                pthread_mutex_unlock(&join_mutex);
            }
            // S
            if ((i + 1 < x) && (map[i + 1][j] == 1)){
                pthread_mutex_lock(&join_mutex);
                join({i, j}, {i + 1, j});
                pthread_mutex_unlock(&join_mutex);
            }
            // SO
            if ((i + 1 < x) && (j - 1 >= 0) && (map[i + 1][j - 1] == 1)){
                pthread_mutex_lock(&join_mutex);
                join({i, j}, {i + 1, j - 1});
                pthread_mutex_unlock(&join_mutex);
            }
            // O
            if ((j - 1 >= 0) && (map[i][j - 1] == 1)){
                pthread_mutex_lock(&join_mutex);
                join({i, j}, {i, j - 1});
                pthread_mutex_unlock(&join_mutex);
            }
        }
    }

    pthread_exit(NULL);
}

int main(){
    int i, j;
    std::ifstream fs;
    fs.open("input.txt");
    fs >> x >> y >> T;

    pthread_t threads[T];
    int ThreadsID[T], rc;
    pthread_barrier_init(&barrier, NULL, T);

    // Alocações
    root = allocMatrix<ii>(x, y);
    sz = allocMatrix<int>(x, y);
    map = allocMatrix<int>(x, y);

    // Inicialização das matrizes e contagem inicial de ilhas
    for (i = 0; i < x; i++){
        for (j = 0; j < y; j++){
            root[i][j] = {i, j};
            sz[i][j] = 0;
            fs >> map[i][j];
            if(map[i][j] == 1){
                numIslands++;
            }
        }
    }

    for (i = 0; i < T; i++){
        ThreadsID[i] = i;
        rc = pthread_create(&threads[i], NULL, findIslands, &ThreadsID[i]);
        printf("Criando thread: %d\n", i);
        if (rc){
            printf("Falhou na criação da thread %d\n", i);
            exit(1);
        }
    }

    for (i = 0; i < T; i++){
        pthread_join(threads[i], NULL);
    }

    printf("Number of islands: %d\n", numIslands);

    freeMatrix<ii>(root, x);
    freeMatrix<int>(sz, x);
    freeMatrix<int>(map, x);
    pthread_barrier_destroy(&barrier);
    pthread_exit(NULL);
    fs.close();
}
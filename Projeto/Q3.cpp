#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>

// Tamanho do labirinto
#define N 4

// Pilha de globais no canto do quarto
std::queue<std::pair<int, int>> q;
int matrix[N][N];
bool visit[N][N];
int distX[4] = {-1, 0, 1, 0};
int distY[4] = {0, 1, 0, -1};
int endX, endY;
int x, y;
std::pair<int, int> temp;

bool isTherePath(){ // BFS
    while(!q.empty()){
        temp = q.front();
        q.pop();

        // Checa se chegou no destino
        if(temp.first == endX && temp.second == endY){
            return true;
        }

        for(int i = 0; i < 4; i++){
            x = temp.first + distX[i];
            y = temp.second + distY[i];

            if(x >= 0 && x < N && y >= 0 && y < N && !visit[x][y]){
                visit[x][y] = true;
                if(matrix[x][y] == 0){
                    q.push({x, y});
                }
            }
        }
    }

    return false;
}

int main(){
    int startX, startY;

    // Inicializa a matriz de células visitadas e lê o labirinto
    std::cout << "Insira a matriz do labirinto:\n";
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
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

    bool ans = isTherePath();

    if(ans == true){
        std::cout << "Existe caminho\n";
    }
    else{
        std::cout << "N existe caminho\n";
    }
}
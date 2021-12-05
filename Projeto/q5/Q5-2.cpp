#include <iostream>
#include <bits/stdc++.h>

typedef std::pair<int, int> ii;

ii **root;
int **sz;
int **map;
int x, y, T;

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
	printf("Checando (%d, %d) e (%d, %d): ", a.first, a.second, b.first, b.second);
    a = find(a);
    b = find(b);


    if (a != b){
        if (sz[a.first][a.second] < sz[b.first][b.second]){
			root[a.first][a.second] = b;
			printf("(%d, %d) agora tem root em (%d, %d)", a.first, a.second, b.first, b.second);
        }
        else{
            root[b.first][b.second] = a;
			printf("(%d, %d) agora tem root em (%d, %d)", b.first, b.second, a.first, a.second);
        }

		sz[a.first][a.second] += sz[b.first][b.second];
		sz[b.first][b.second] += sz[a.first][a.second];
    }
	else{
		printf("Nada mudou nessa porra.");
	}
	printf("\n");
}

void findIslands(){
    for (int i = 0; i < x; i ++){
        for (int j = 0; j < y; j++){
            if (map[i][j] == 0){
                continue;
            }
            // N
            if ((i - 1 >= 0) && (map[i - 1][j] == 1)){
                join({i, j}, {i - 1, j});
            }
            // NE
            if ((i - 1 >= 0) && (j + 1 < y) && (map[i - 1][j + 1] == 1)){
                join({i, j}, {i - 1, j + 1});
            }
            // E
            if ((j + 1 < y) && (map[i][j + 1] == 1)){
                join({i, j}, {i, j + 1});
            }
            // SE
            if ((i + 1 < x) && (j + 1 < y) && (map[i + 1][j + 1] == 1)){
                join({i, j}, {i + 1, j + 1});
            }
            // S
            if ((i + 1 < x) && (map[i + 1][j] == 1)){
                join({i, j}, {i + 1, j});
            }
            // SO
            if ((i + 1 < x) && (j - 1 >= 0) && (map[i + 1][j - 1] == 1)){
                join({i, j}, {i + 1, j - 1});
            }
            // O
            if ((j - 1 >= 0) && (map[i][j - 1] == 1)){
                join({i, j}, {i, j - 1});
            }
            // NO
            if ((i - 1 >= 0) && (j - 1 >= 0) && (map[i - 1][j - 1] == 1)){
                join({i, j}, {i - 1, j - 1});
            }
        }
    }

    // for (int i = 1; i < x; i += 2)
    // {
    //     for (int j = 0; j < y; j++)
    //     {
    //         if (map[i][j] == 0)
    //         {
    //             continue;
    //         }
    //         // N
    //         if ((i - 1 >= 0) && (map[i - 1][j] == 1))
    //         {
    //             join({i, j}, {i - 1, j});
    //         }
    //         // NE
    //         if ((i - 1 >= 0) && (j + 1 < y) && (map[i - 1][j + 1] == 1))
    //         {
    //             join({i, j}, {i - 1, j + 1});
    //         }
    //         // E
    //         if ((j + 1 < y) && (map[i][j + 1] == 1))
    //         {

    //             join({i, j}, {i, j + 1});
    //         }
    //         // SE
    //         if ((i + 1 < x) && (j + 1 < y) && (map[i + 1][j + 1] == 1))
    //         {

    //             join({i, j}, {i + 1, j + 1});
    //         }
    //         // S
    //         if ((i + 1 < x) && (map[i + 1][j] == 1))
    //         {

    //             join({i, j}, {i + 1, j});
    //         }
    //         // SO
    //         if ((i + 1 < x) && (j - 1 >= 0) && (map[i + 1][j - 1] == 1))
    //         {

    //             join({i, j}, {i + 1, j - 1});
    //         }
    //         // O
    //         if ((j - 1 >= 0) && (map[i][j - 1] == 1))
    //         {

    //             join({i, j}, {i, j - 1});
    //         }
    //         // NO
    //         if ((i - 1 >= 0) && (j - 1 >= 0) && (map[i - 1][j - 1] == 1))
    //         {
    //             join({i, j}, {i - 1, j - 1});
    //         }
    //     }
    // }
}

int main(){
    int i, j;
    std::cin >> x >> y;

    root = allocMatrix<ii>(x, y);
    sz = allocMatrix<int>(x, y);
    map = allocMatrix<int>(x, y);

    for (i = 0; i < x; i++){
        for (j = 0; j < y; j++){
            root[i][j] = {i, j};
            sz[i][j] = 0;
            std::cin >> map[i][j];
        }
    }

    findIslands();

    for (i = 0; i < x; i++){
        for (j = 0; j < y; j++){
            std::cout << "(" << root[i][j].first << ", " << root[i][j].second << ") ";
        }
        std::cout << "\n";
    }

    freeMatrix<ii>(root, x);
    freeMatrix<int>(sz, x);
    freeMatrix<int>(map, x);
}
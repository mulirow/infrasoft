#define _XOPEN_SOURCE 600
#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>

typedef std::pair<int, int> ii;
#define NUM_THREADS 4
pthread_barrier_t barrier;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

ii **root;
int **sz;
int **map;
int x, y, T;

template <typename T>
T **allocMatrix(int height, int width)
{
    int i, j;
    T **matrix = (T **)malloc(height * sizeof(T *));
    if (matrix == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }
    for (i = 0; i < height; i++)
    {
        matrix[i] = (T *)malloc(width * sizeof(T));
        if (matrix[i] == NULL)
        {
            printf("Failed to allocate memory.\n");
            exit(1);
        }
    }

    return matrix;
}

template <typename T>
void freeMatrix(T **matrix, int height)
{
    int i;
    for (i = 0; i < height; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

ii find(ii a)
{
    if (root[a.first][a.second] == a)
    {
        return a;
    }
    return find(root[a.first][a.second]);
}

void join(ii a, ii b)
{
    a = find(a);
    b = find(b);

    if (a != b)
    {
        if ((a.first > b.first) && (a.second > b.second))
        {
            root[a.first][a.second] = b;
        }
        else
        {
            root[b.first][b.second] = a;
        }
    }
}

void *findIslands(void *threadid)
{
    int ID = (*(int *)threadid);

    for (int i = 2 * ID; i < x; i += 2 * T)
    {
        for (int j = 0; j < y; j++)
        {
            if (map[i][j] == 0)
            {
                continue;
            }
            // //N
            // if((i - 1 >= 0) && (map[i-1][j] == 1)){
            //     join({i, j}, {i-1, j});
            // }
            // //NE
            // if((i - 1 >= 0) && (j + 1 < y) && (map[i-1][j+1] == 1)){
            //     join({i, j}, {i-1, j+1});
            // }
            // E
            if ((j + 1 < y) && (map[i][j + 1] == 1))
            {
                pthread_mutex_lock(&mymutex);
                join({i, j}, {i, j + 1});
                pthread_mutex_unlock(&mymutex);
            }
            // SE
            if ((i + 1 < x) && (j + 1 < y) && (map[i + 1][j + 1] == 1))
            {
                pthread_mutex_lock(&mymutex);
                join({i, j}, {i + 1, j + 1});
                pthread_mutex_unlock(&mymutex);
            }
            // S
            if ((i + 1 < x) && (map[i + 1][j] == 1))
            {
                pthread_mutex_lock(&mymutex);
                join({i, j}, {i + 1, j});
                pthread_mutex_unlock(&mymutex);
            }
            // SO
            if ((i + 1 < x) && (j - 1 >= 0) && (map[i + 1][j - 1] == 1))
            {
                pthread_mutex_lock(&mymutex);
                join({i, j}, {i + 1, j - 1});
                pthread_mutex_unlock(&mymutex);
            }
            // O
            if ((j - 1 >= 0) && (map[i][j - 1] == 1))
            {
                pthread_mutex_lock(&mymutex);
                join({i, j}, {i, j - 1});
                pthread_mutex_unlock(&mymutex);
            }
            // //NO
            // if((i - 1 >= 0) && (j - 1 >= 0) && (map[i-1][j-1] == 1)){
            //     join({i, j}, {i-1, j-1});
            // }
        }
    }

    pthread_barrier_wait(&barrier);

    for (int i = 2 * ID + 1; i < x; i += 2 * T)
    {
        for (int j = 0; j < y; j++)
        {
            if (map[i][j] == 0)
            {
                continue;
            }
            // //N
            // if((i - 1 >= 0) && (map[i-1][j] == 1)){
            //     join({i, j}, {i-1, j});
            // }
            // //NE
            // if((i - 1 >= 0) && (j + 1 < y) && (map[i-1][j+1] == 1)){
            //     join({i, j}, {i-1, j+1});
            // }
            // E
            if ((j + 1 < y) && (map[i][j + 1] == 1))
            {
                pthread_mutex_lock(&mymutex);
                join({i, j}, {i, j + 1});
                pthread_mutex_unlock(&mymutex);
            }
            // SE
            if ((i + 1 < x) && (j + 1 < y) && (map[i + 1][j + 1] == 1))
            {
                pthread_mutex_lock(&mymutex);
                join({i, j}, {i + 1, j + 1});
                pthread_mutex_unlock(&mymutex);
            }
            // S
            if ((i + 1 < x) && (map[i + 1][j] == 1))
            {
                pthread_mutex_lock(&mymutex);
                join({i, j}, {i + 1, j});
                pthread_mutex_unlock(&mymutex);
            }
            // SO
            if ((i + 1 < x) && (j - 1 >= 0) && (map[i + 1][j - 1] == 1))
            {
                pthread_mutex_lock(&mymutex);
                join({i, j}, {i + 1, j - 1});
                pthread_mutex_unlock(&mymutex);
            }
            // O
            if ((j - 1 >= 0) && (map[i][j - 1] == 1))
            {
                pthread_mutex_lock(&mymutex);
                join({i, j}, {i, j - 1});
                pthread_mutex_unlock(&mymutex);
            }
            // //NO
            // if((i - 1 >= 0) && (j - 1 >= 0) && (map[i-1][j-1] == 1)){
            //     join({i, j}, {i-1, j-1});
            // }
        }
    }

    pthread_exit(NULL);
}

int main()
{
    int i, j;
    std::cin >> x >> y >> T;

    pthread_t Threads[T];
    int ThreadsID[T], rc;

    pthread_barrier_init(&barrier, NULL, T);

    root = allocMatrix<ii>(x, y);
    sz = allocMatrix<int>(x, y);
    map = allocMatrix<int>(x, y);

    for (i = 0; i < x; i++)
    {
        for (j = 0; j < y; j++)
        {
            root[i][j] = {i, j};
            sz[i][j] = 0;
            std::cin >> map[i][j];
        }
    }

    for (i = 0; i < T; i++)
    {
        ThreadsID[i] = i;
        rc = pthread_create(&Threads[i], NULL, findIslands, &ThreadsID[i]);
        printf("Criando Thread: %d\n", i);
        if (rc)
        {
            printf("Falhou na criação da Thread %d\n", i);
            exit(1);
        }
    }

    for (i = 0; i < T; i++)
    {
        pthread_join(Threads[i], NULL);
    }

    for (i = 0; i < x; i++)
    {
        for (j = 0; j < y; j++)
        {
            std::cout << "(" << root[i][j].first << ", " << root[i][j].second << ") ";
        }
        std::cout << "\n";
    }

    freeMatrix<ii>(root, x);
    freeMatrix<int>(sz, x);
    freeMatrix<int>(map, x);
    pthread_barrier_destroy(&barrier);
    pthread_exit(NULL);
}
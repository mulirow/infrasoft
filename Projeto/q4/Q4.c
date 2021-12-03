#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

//gray ficou na struct ao inves de uma matriz separada por preguica de fazer outra funcao de alocacao kkkkkk
typedef struct{
    int r;
    int g;
    int b;
    int gray;
} Pixel;

int x, y;
Pixel** bitmap;

Pixel** allocMatrix(int height, int width){
    int i, j;
    Pixel** matrix = (Pixel **) malloc (height * sizeof(Pixel*));
    if(matrix == NULL){
        printf("Failed to allocate memory.\n");
        exit(1);
    }
    for(i = 0; i < height; i++){
        matrix[i] = (Pixel *) malloc (width * sizeof(Pixel));
        if(matrix[i] == NULL){
            printf("Failed to allocate memory.\n");
            exit(1);
        }
    }

    return matrix;
}

void freeMatrix(Pixel** matrix, int height){
    int i;
    for(i = 0; i < height; i++){
        free(matrix[i]);
    }
    free(matrix);
}

void *convertToGray(void *tid){
    int threadID = (*(int *)tid);

    //Isolamento de linhas para que não haja condição de disputa entre threads
    for(int i = threadID; i < x; i += NUM_THREADS){
        for(int j = 0; j < y; j++){
            bitmap[i][j].gray = bitmap[i][j].r * 0.3 + bitmap[i][j].g * 0.59 + bitmap[i][j].b * 0.11;
        }
    }
}

int main(){
    //Abrir arquivos
    FILE *input = fopen("input.txt", "r");
    if(input == NULL){
        printf("Nao foi possivel abrir o input. Tenha certeza que ele está nomeado como input.txt.\n");
        exit(1);
    }
    FILE *output = fopen("output.txt", "w");
    if(output == NULL){
        printf("Nao foi possivel abrir o arquivo de output.\n");
        exit(1);
    }

    //Checar formato
    char format[10];
    fscanf(input, " %s", format);
    if(strcmp("P3", format) != 0){
        printf("O arquivo possui especificador \"%s\", mas \"P3\" era esperado.\n", format);
        exit(1);
    }

    int limit;
    fscanf(input, "%d", &limit);

    //Alocar matriz
    fscanf(input, "%d %d", &x, &y);
    bitmap = allocMatrix(x, y);

    //Receber dados do arquivo
    for(int i = 0; i < x; i++){
        for(int j = 0; j < y; j++){
            fscanf(input, "%d %d %d", &bitmap[i][j].r, &bitmap[i][j].g, &bitmap[i][j].b);
            if(bitmap[i][j].r > limit){
                printf("Valor R do pixel [%d][%d] passou do valor limite. Ele sera auto-definido para o limite.\n", i, j);
                bitmap[i][j].r = limit;
            }
            if(bitmap[i][j].g > limit){
                printf("Valor G do pixel [%d][%d] passou do valor limite. Ele sera auto-definido para o limite.\n", i, j);
                bitmap[i][j].g = limit;
            }
            if(bitmap[i][j].b > limit){
                printf("Valor B do pixel [%d][%d] passou do valor limite. Ele sera auto-definido para o limite.\n", i, j);
                bitmap[i][j].b = limit;
            }
        }
    }

    pthread_t threads[NUM_THREADS];
    int threadID[NUM_THREADS];
    int rc;

    for(int i = 0; i < NUM_THREADS; i++){
        threadID[i] = i;
        printf("Criando Thread: %d\n", i);
        rc = pthread_create(&threads[i], NULL, convertToGray, &threadID[i]);
        if(rc){
            printf("Fomos de Base na %da Thread\n", i);
            exit(1);
        }
    }

    for(int i = 0; i < NUM_THREADS;i++){
        pthread_join(threads[i], NULL);
    }

    //Passar resultados para o arquivo de output
    for(int i = 0; i < x; i++){
        for(int j = 0; j < y; j++){
            fprintf(output, "%d %d %d\n", bitmap[i][j].gray, bitmap[i][j].gray, bitmap[i][j].gray);
        }
    }

    fclose(input);
    fclose(output);
    freeMatrix(bitmap, x);
    pthread_exit(NULL);
}
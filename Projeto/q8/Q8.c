#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

// Maximum number of passenger threads available
#define PASSAGEIROS 21
#define VOLTAS 10
#define CAPACIDADE 10

// Mutexes para controlar o processo de embarque e desembarque
pthread_mutex_t check_in_lock; 
pthread_mutex_t riding_lock; 	

sem_t board_queue; 					// Semáforo que garante o embarque da thread dos passageiros
sem_t check_passenger;				// Semáforo que garante que o próximo passageiro entre após a entrada do último
sem_t all_boarded; 					// Semáforo binário para bloquear o passageiro de entrar
sem_t unboard_queue;				// Semáfoeo que garante o desembarque da thread dos passagei
sem_t all_unboarded; 				// Semáforo binário para permitir o passageiro de entrar

int boarded = 0; 					// Número de passageiros que entraram no carrinho
int unboarded = 0; 					// Número de passageiros que saíram do carrinho
int current_ride = 0; 				// Número de voltas dadas
int total_rides; 					// Número máximo de voltas
int passengers; 					// Numéro de threads de passageiros
int capacity; 						// Capacidade da thread do carrinho
int status[PASSAGEIROS] = {0};		// Estado dos passageiros para que não entrem duas vezes seguidas

void* carThread(void *threadid){
	int i;
	// Roda o carrinho, até dar o número máximo de voltas
	while(current_ride < total_rides){
		printf("A volta %d irá começar, hora de embarcar!\n", current_ride+1);
		printf("A capacidade do carrinho é de %d passageiros!\n", capacity);

		// Atualiza o estado dos passageiros
		// Região crítica: status[]
		pthread_mutex_lock(&check_in_lock);
		for(i = 0; i < PASSAGEIROS; i++){
			if(status[i] > 0){
				status[i]--;
			}
		}
		pthread_mutex_unlock(&check_in_lock);

		// Um passageiro entra por vez...
		while(boarded < CAPACIDADE){
			sem_post(&board_queue);
			sem_wait(&check_passenger);
		}
		// ...Até que a capacidade máxima seja atingida
		sem_wait(&all_boarded);

		// Zera a quantidade de embarcados para a próxima iteração
		pthread_mutex_lock(&check_in_lock);
		boarded = 0;
		pthread_mutex_unlock(&check_in_lock);

		printf("O carrinho encheu, hora de rodar!\n");
		printf("O carrinho está rodando!\n");
		printf("Acabou a volta, hora de sair!\n");

		// Avisa o passageiro que pode sair do carrinho
		for(i = 0; i < CAPACIDADE; i++)	sem_post(&unboard_queue);

		// Espera que todos os passageiros saiam do carrinho
		sem_wait(&all_unboarded);

		printf("Todos saíram, o carrinho está vazio!\n\n");

		current_ride++;
	}
	return NULL;
}

void* passengerThread(void *threadid){
    int ID = (*(int *)threadid);
	// Ocorre até o final do processo, quando as threads vão para o destroyer
	while(1){
		// Aguarda a sinalização para chamar o passageiro
		sem_wait(&board_queue);
		// Região crítica: status[] e boarded
		pthread_mutex_lock(&check_in_lock);
		// Se o passageiro escalonado já tentou ir na última volta, só chama o próximo
		if(status[ID] != 0){
			pthread_mutex_unlock(&check_in_lock);
			sem_post(&check_passenger);
			continue;
		}
		// Caso o passageiro possa entrar no carrinho, ele entra
		boarded++;
		status[ID] = 2;
		printf("O passageiro %d entrou do carrinho\n", ID);

		// Se o carrinho está cheio, ele inicia a volta
		if (boarded == capacity){
			sem_post(&all_boarded);
		}
		// Lembrando de indicar que a chamada do próximo passageiro está disponível
		sem_post(&check_passenger);
		// Fim de uso da região crítica
		pthread_mutex_unlock(&check_in_lock);

		// Aguarda o fim da volta
		sem_wait(&unboard_queue);

		// Região crítica: unboarded
		pthread_mutex_lock(&riding_lock);
		unboarded++;
		printf("O passageiro %d saiu do carrinho\n", ID);
		// Se o carrinho está vazio, vamos para a próxima volta
		if (unboarded == capacity){
			sem_post(&all_unboarded);
			unboarded = 0;
		}
		pthread_mutex_unlock(&riding_lock);
	}
    return NULL;
}

int main(){

	passengers = 21;
	capacity = 10;
	total_rides = 10;

	pthread_t passenger[passengers];
	pthread_t car;
	int i, passengers_ID[passengers];

	// Novos Mutexes e Semáforos
	pthread_mutex_init(&check_in_lock, NULL);
	pthread_mutex_init(&riding_lock, NULL);
	sem_init(&board_queue, 0, 0);
	sem_init(&all_boarded, 0, 0);
	sem_init(&unboard_queue, 0, 0);
	sem_init(&all_unboarded, 0, 0);
	sem_init(&check_passenger, 0, 0);

	printf("O carrinho irá dar %d voltas!\n", total_rides);
	printf("Tem %d pessoas na fila!\n\n", passengers);

	// Criando as threads e iniciando o processo da montanha-russa
	pthread_create(&car, NULL, carThread, NULL);
	for(i = 0; i < passengers; i++){
        passengers_ID[i] = i;
        pthread_create(&passenger[i], NULL, passengerThread, &passengers_ID[i]);

    } 
	// Entra na thread do carrinho, após as 10 voltas forem completas
	pthread_join(car, NULL);

	printf("Todas as voltas foram dadas, até a próxima!\n");

	// Destroyers
	pthread_mutex_destroy(&check_in_lock);
	pthread_mutex_destroy(&riding_lock);
	sem_destroy(&board_queue);
	sem_destroy(&all_boarded);
	sem_destroy(&unboard_queue);
	sem_destroy(&all_unboarded);

	return 0;
} 
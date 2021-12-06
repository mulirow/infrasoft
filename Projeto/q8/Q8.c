#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

// Maximum number of passenger threads available
#define PASSAGEIROS 20
#define VOLTAS 10

/* Variables */
pthread_mutex_t check_in_lock; 	// mutex to control access of the 'boarded' variable
pthread_mutex_t riding_lock; 		// mutex to control access of the 'unboarded' variable

sem_t embarque; 							// semaphore to ensure boarding of C passenger threads
sem_t embarcados; 							// binary semaphore to tell passenger threads to wait for the next ride
sem_t desembarque;						// semaphore to ensure unboarding of C passenger threads
sem_t desembarcados; 						// binary semaphore to signal passenger threads for boarding

volatile int boarded = 0; 			// current number of passenger threads that have boarded
volatile int unboarded = 0; 		// current number of passenger threads that have unboarded
volatile int current_ride = 0; 	// current number of rides
volatile int total_rides; 			// total number of coaster rides for the instance
volatile int passengers; 				// current number of passenger threads
volatile int capacity; 					// current capacity of the car thread

/* Helper functions */
void load(){
	printf("Volta #%d irá começar, embarcando!\n", current_ride+1);
	printf("A capacidade do carrinho é de %d pessoas\n", capacity);
}
void run(){
	printf("\nO carrinho está cheio, começando a volta!\n");
	printf("O carrinho está dando a volta!\n");
}
void unload(){
	printf("\nAcabou a volta, desembarcando!\n");
}
void board(){
	printf("%d passageiros entraram no carrinho...\n", boarded);
}
void unboard(){
	printf("%d passageiros saíram do carrinho...\n", unboarded);
}

/* Thread Functions */
void* carThread(void *threadID){
	int i;
	// Run the roller coaster for <total_rides> times
	while(current_ride < total_rides){
		load();
		
		for(i = 0; i <= capacity; i++) sem_post(&embarque); // Signal C passenger threads to board the car
		sem_wait(&embarcados); // Wait for all passengers to board
		
		run();
		unload();
		
		for(i = 0; i <= capacity; i++) sem_post(&desembarque); // Signal the passengers in the car to unboard
		sem_wait(&desembarcados); // Tell the queue to start boarding again
		printf("O carrinho está vazio!\n\n");
		
		current_ride++;
	}
	return NULL;
}

void* passengerThread(void *threadID){
	// Run indefinitely, threads will be destroyed when the main process exits
	while(1){
		sem_wait(&embarque); // Wait for the car thread signal to board
		
		pthread_mutex_lock(&check_in_lock); // Lock access to shared variable before incrementing
		boarded++;
		board();
		if (boarded == capacity){
			sem_post(&embarcados); // If this is the last passenger to board, signal the car to run

			if(boarded == 19)
				boarded = 0;
		}
		pthread_mutex_unlock(&check_in_lock); // Unlock access to shared variable

		sem_wait(&desembarque); // Wait for the ride to end
	
		pthread_mutex_lock(&riding_lock); // Lock access to shared variable before incrementing
		unboarded++;
		unboard();
		if (unboarded == capacity){
			sem_post(&desembarcados); // If this is the last passenger to unboard, signal the car to allow boarding

			if(unboarded == 19)
				unboarded = 0;
		}
		pthread_mutex_unlock(&riding_lock); // Unlock access to shared variable
	}
    	return NULL;
}

/* Main program */
int main() {
	// Set new instance of passenger threads, car capacity and total rides values
	passengers = 20;
	capacity = 10;
	total_rides = 10;

	pthread_t passenger[passengers];
	pthread_t car;
	int i;
	
	// Create new mutexes and semaphores
	pthread_mutex_init(&check_in_lock, NULL);
	pthread_mutex_init(&riding_lock, NULL);
	sem_init(&embarque, 0, 0);
	sem_init(&embarcados, 0, 0);
	sem_init(&desembarque, 0, 0);
	sem_init(&desembarcados, 0, 0);

	printf("A montanha-russa da %d voltas por dia!\n", total_rides);
	printf("Têm %d passageiros na fila!\n\n", passengers);
	
	// Cria threads e inicia a montanha-russa
	pthread_create(&car, NULL, carThread, NULL);
	for(i = 0; i < passengers; i++) pthread_create(&passenger[i], NULL, passengerThread, NULL);
	// Entra na thread do carro quando todas as 10 voltas foram dadas
	pthread_join(car, NULL); 		
	
	printf("Todas as voltas diárias foram dadas, por hoje é só!!.\n");

	// Destroyers
	pthread_mutex_destroy(&check_in_lock);
	pthread_mutex_destroy(&riding_lock);
	sem_destroy(&embarque);
	sem_destroy(&embarcados);
	sem_destroy(&desembarque);
	sem_destroy(&desembarcados);

	return 0;
}
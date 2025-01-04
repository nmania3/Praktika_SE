#include "../include/threads.h"
#include <stdio.h>
#include "../include/data_structures.h"
#include <pthread.h>
#include <unistd.h> // Para sleep

extern pthread_cond_t cond;
extern pthread_mutex_t mutex_clock;
extern int clock_frequency;

void *clock_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex_clock);
        //printf("Tick del reloj\n");
        pthread_cond_broadcast(&cond); // Notifica a los temporizadores
        pthread_mutex_unlock(&mutex_clock);
        
        sleep(clock_frequency); // Marca el ritmo del reloj
    }
}

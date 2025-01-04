#include "../include/threads.h"
#include <stdio.h>
#include "../include/data_structures.h"
#include <pthread.h>
#include <unistd.h> 

extern pthread_cond_t cond;
extern pthread_cond_t cond_gen;
extern pthread_cond_t cond_sched;
extern pthread_mutex_t mutex_clock;
extern pthread_mutex_t queue_mutex;
extern int timer_frequency;

void *timer_thread1(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex_clock);
        pthread_cond_wait(&cond, &mutex_clock); // Espera el tick del reloj
        //printf("Soy el temporizador 1\n");
        
        // Notifica al generador de procesos
        pthread_mutex_lock(&queue_mutex);
        pthread_cond_signal(&cond_gen);
        pthread_mutex_unlock(&queue_mutex);

        pthread_mutex_unlock(&mutex_clock);
        sleep(timer_frequency);
    }
}

void *timer_thread2(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex_clock);
        pthread_cond_wait(&cond, &mutex_clock); // Espera el tick del reloj
        //printf("Soy el temporizador 2\n");

        // Notifica al planificador
        pthread_mutex_lock(&queue_mutex);
        pthread_cond_signal(&cond_sched);
        pthread_mutex_unlock(&queue_mutex);

        pthread_mutex_unlock(&mutex_clock);
        sleep(timer_frequency);
    }
}

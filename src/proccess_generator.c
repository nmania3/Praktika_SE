#include "../include/threads.h"
#include <stdio.h>
#include "../include/data_structures.h"
#include <pthread.h>
#include <unistd.h> // Para sleep

struct PCB {
    int pid;
    // Otros campos si es necesario
};

extern pthread_cond_t cond_gen;
extern pthread_mutex_t queue_mutex;

void *proccess_generator_thread(void* arg) {
    int pid = 1;

    while (1) {
        pthread_mutex_lock(&queue_mutex);
        pthread_cond_wait(&cond_gen, &queue_mutex); // Espera señal del timer 1
        printf("Proceso generado con PID: %d\n", pid++);
        pthread_mutex_unlock(&queue_mutex);
    }
}

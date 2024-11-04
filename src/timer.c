#include "../include/threads.h"
#include <stdio.h>
#include "../include/data_structures.h"
#include <pthread.h>

void *timer_thread1(void *arg) {
    pthread_mutex_lock(&mutex_clock);
    while (1) {
        done++;
        printf("soy el temporizador 1 \n");
        pthread_cond_signal(&cond);
        pthread_cond_wait(&cond2, &mutex_clock);
    }
}
void *timer_thread2(void *arg) {
    pthread_mutex_lock(&mutex_clock);
    while (1) {
        done++;
        printf("soy el temporizador 2 \n");
        pthread_cond_signal(&cond);
        pthread_cond_wait(&cond2, &mutex_clock);
    }
}


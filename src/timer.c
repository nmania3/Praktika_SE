#include "../include/threads.h"
#include <stdio.h>
#include "../include/data_structures.h"
#include <pthread.h>

void *timer_thread(void *arg) {
    pthread_mutex_lock(&mutex_clock);
    while (1) {
        
        printf("soy el temporizador %d\n", done);
        sleep(timer_frequency);
        done++;
        printf("soy el temporizador %d\n", done);
        pthread_cond_signal(&cond);
        pthread_cond_wait(&cond2, &mutex_clock);
    }
}
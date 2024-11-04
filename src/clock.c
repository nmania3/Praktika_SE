#include "../include/threads.h"
#include <stdio.h>
#include "../include/data_structures.h"
#include <pthread.h>



void *clock_thread(void *arg) {
    while (1)
    {
        
        pthread_mutex_lock(&mutex_clock);
        while (done<temp_kop)
        {
            pthread_cond_wait(&cond, &mutex_clock);
            printf("soy el reloj %d\n", done);
            
        }
        done=0;
        sleep(clock_frequency);
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex_clock);
    }
    
    
}
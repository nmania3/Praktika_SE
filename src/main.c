#include <stdio.h>
#include <pthread.h>
#include "../include/threads.h"
#include "../include/data_structures.h"

int timer_frequency=1; //ahora mismo no lo usamos
int clock_frequency=1;
int temp_kop=2;
int done=0;

pthread_mutex_t mutex_clock;
pthread_cond_t cond;
pthread_cond_t cond2;

int main(int argc, char const *argv[])
{
    pthread_t clock_tid, timer_tid1, timer_tid2;

    pthread_mutex_init(&mutex_clock, NULL);

    pthread_create(&clock_tid, NULL, clock_thread, NULL);
    pthread_create(&timer_tid1, NULL, timer_thread1, NULL);
    pthread_create(&timer_tid2, NULL, timer_thread2, NULL);

    pthread_join(clock_tid, NULL);
    pthread_join(timer_tid1, NULL);
    pthread_join(timer_tid2, NULL);

    pthread_mutex_destroy(&mutex_clock);

    return 0;

}
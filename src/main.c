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
    pthread_t clock_tid, timer_tid1, timer_tid2, scheduler_tid, proccess_generator_tid;

    pthread_mutex_init(&mutex_clock, NULL);

    pthread_create(&clock_tid, NULL, clock_thread, NULL);
    pthread_create(&timer_tid1, NULL, timer_thread1, NULL);
    pthread_create(&timer_tid2, NULL, timer_thread2, NULL);
    pthread_create(&scheduler_tid, NULL, scheduler_thread, NULL);
    pthread_create(&proccess_generator_tid, NULL, proccess_generator_thread, NULL);

    pthread_join(clock_tid, NULL);
    pthread_join(timer_tid1, NULL);
    pthread_join(timer_tid2, NULL);
    pthread_join(scheduler_tid, NULL);
    pthread_join(proccess_generator_tid, NULL);

    pthread_mutex_destroy(&mutex_clock);

    return 0;

}
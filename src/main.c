#include <stdio.h>
#include <pthread.h>
#include "../include/threads.h"
#include "../include/data_structures.h"

timer_frequency=1;
clock_frequency=1;
temp_kop=1;
done=0;

pthread_mutex_t mutex_clock;
pthread_cond_t cond;
pthread_cond_t cond2;

int main(int argc, char const *argv[])
{
    pthread_t clock_tid, timer_tid;

    pthread_mutex_init(&mutex_clock, NULL);

    pthread_create(&clock_tid, NULL, clock_thread, NULL);
    pthread_create(&timer_tid, NULL, timer_thread, NULL);

    pthread_join(clock_tid, NULL);
    pthread_join(timer_tid, NULL);

    pthread_mutex_destroy(&mutex_clock);

    return 0;

}
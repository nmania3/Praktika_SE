#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "../include/threads.h"
#include "../include/data_structures.h"

int timer_frequency=1;
int clock_frequency=1;
int proccess_generator_frequency=1;

struct CPU
{
    int cpu_num;
    int core_num;
    int thread_num;
};

int temp_kop=2;
int done=0;

pthread_mutex_t mutex_clock;
pthread_mutex_t queue_mutex;

pthread_cond_t cond;
pthread_cond_t cond2;
pthread_cond_t cond_gen;

int main(int argc, char const *argv[])
{

    if (argc != 7)
    {
        fprintf(stderr, "Usage: %s <timer_frequency> <clock_frequency> <proccess_generator_frequency> <cpu_num> <core_num> <thread_num>\n", argv[0]);
        fprintf(stderr, "Default: %s 0 1 0 1 1 1\n", argv[0]);
        return 1;
    }

    timer_frequency = atoi(argv[1]);
    clock_frequency = atoi(argv[2]);
    proccess_generator_frequency = atoi(argv[3]);

    struct CPU cpu;
    cpu.cpu_num = atoi(argv[4]);
    cpu.core_num = atoi(argv[5]);
    cpu.thread_num = atoi(argv[6]);

    printf("timer_frequency: %d\n", timer_frequency);
    printf("clock_frequency: %d\n", clock_frequency);
    printf("proccess_generator_frequency: %d\n", proccess_generator_frequency);
    printf("cpu_num: %d\n", cpu.cpu_num);
    printf("core_num: %d\n", cpu.core_num);
    printf("thread_num: %d\n", cpu.thread_num);

    printf("\n ------------------------------------ \n");

    pthread_t clock_tid, timer_tid1, timer_tid2, scheduler_tid, proccess_generator_tid;

    pthread_mutex_init(&mutex_clock, NULL);
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&cond_gen, NULL);

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
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&cond_gen);

    return 0;

}
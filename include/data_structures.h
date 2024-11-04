#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <pthread.h>

extern int temp_kop;
extern int clock_frequency;     // frecuencia de tick en segundos
extern int timer_frequency;     // frecuencia de señal del temporizador

extern int tenp_kop;
extern int done;

extern pthread_mutex_t mutex_clock;
extern pthread_cond_t cond;
extern pthread_cond_t cond2;

struct CPU
{
    int cpu_num;
    int core_num;
    int thread_num;
};

struct PCB
{
    int pid;
    //luego meteremos mas cosas
};


#endif // DATA_STRUCTURES_H
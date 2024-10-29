#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <pthread.h>

extern int temp_kop;
extern int clock_frequency;     // frecuencia de tick en segundos
extern int timer_frequency;     // frecuencia de señal del temporizador

extern int tenp_kop;
extern int done;

extern pthread_mutex_t mutex_clock;

#endif // DATA_STRUCTURES_H
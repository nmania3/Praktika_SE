#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H
#include <pthread.h>

extern int temp_kop;
extern int clock_frequency;     // frecuencia de tick en segundos
extern int timer_frequency;     // frecuencia de señal del temporizador

extern int tenp_kop;
extern int done;

extern pthread_mutex_t mutex_clock;
extern pthread_mutex_t queue_mutex;

extern pthread_cond_t cond;
extern pthread_cond_t cond_sched;
extern pthread_cond_t cond_gen;

typedef struct 
{
    int cpu_num;
    int core_num;
    int thread_num;
} CPU;

// Define the PCB structure
typedef struct PCB {
    int pid;
    struct PCB* next;
    char* state;
    float execution_time;
    float completion_time;
} PCB;

// Declare the process queue
extern PCB* process_queue;

extern struct CPU cpu;

#endif // DATA_STRUCTURES_H
#include "../include/threads.h"
#include <stdio.h>
#include "../include/data_structures.h"
#include <unistd.h> 
#include <time.h>
#include <stdlib.h> // Added for malloc
#include <string.h> // Added for strcmp

extern pthread_cond_t cond_sched;
extern pthread_mutex_t queue_mutex;

int last_pid = 0;

extern struct CPU
{
    int cpu_num;
    int core_num;
    int thread_num;
};

extern struct CPU cpu;

PCB* exec_queue[10]; // Define with a fixed size
pthread_t *threads; // Declare threads globally

void *proccess_execution_fcfs(void* arg); // Update function signature

void *scheduler_thread(void* arg) {
    threads = malloc(cpu.thread_num * sizeof(pthread_t)); // Allocate threads array
    //crear tantos hilos como thread_num
    while(1){
        pthread_mutex_lock(&queue_mutex);
        pthread_cond_wait(&cond_sched, &queue_mutex); // Espera señal del timer 2
        
        printf("soy el planificador\n");
        
        politica_fcfs();
        pthread_mutex_unlock(&queue_mutex);
        sleep(1);
    }
    
    
    return 0;
}

void politica_fcfs(){

    //Sacar el primer proceso de la cola con stado "READY"
    PCB* pcb = process_queue;
    while(pcb != NULL){
        if(strcmp(pcb->state, "READY") == 0){ // Use strcmp for string comparison
            break;
        }
        pcb = pcb->next;
    }
    //si hay espacio en exec_queue ponerlo en exec_queue
    for(int i = 0; i < cpu.thread_num; i++){
        if(exec_queue[i] == NULL){
            exec_queue[i] = pcb; // Store pointer
            //ejecutar el proceso en un hilo
            pthread_create(&threads[i], NULL, proccess_execution_fcfs, exec_queue[i]);
            break;
        }
    }
    //vaciar los procesos terminados de exec_queue
    for(int i = 0; i < cpu.thread_num; i++){
        if(exec_queue[i] != NULL && strcmp(exec_queue[i]->state, "FINISHED") == 0){ // Use -> for pointer access
            exec_queue[i] = NULL;
        }
    }
}

void *proccess_execution_fcfs(void* arg){
    PCB* pcb = (PCB*)arg;
    pcb->state = "EXEC";
    printf("Proceso con PID: %d en ejecucion\n", pcb->pid);
    pcb->completion_time = pcb->execution_time;
    sleep(pcb->execution_time);
    pcb->state = "FINISHED";
    printf("Proceso con PID: %d terminado\n", pcb->pid);
    return NULL;
}
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

PCB* exec_queue[10] = {NULL}; // Initialize with NULL
pthread_t *threads = NULL; // Initialize to NULL

void *proccess_execution_fcfs(void* arg); // Update function signature
void *proccess_execution_round_robin(void* arg); // Update function signature
void *proccess_block(void* arg); // Update function signature

//Variables para round robin
int quantum = 3;
PCB* listaDeTurnos[10];
pthread_t *block_threads;


void *scheduler_thread(void* arg) {
    //actualiza el tamaño de exec_queue a la cantidad de hilos
    // exec_queue[cpu.thread_num]; // This line is not needed

    threads = malloc(cpu.thread_num * sizeof(pthread_t)); // Allocate threads array
    if (threads == NULL) {
        perror("Failed to allocate threads array");
        exit(EXIT_FAILURE);
    }

    //crear tantos hilos como thread_num
    while(1){
        pthread_mutex_lock(&queue_mutex);
        pthread_cond_wait(&cond_sched, &queue_mutex); // Espera señal del timer 2
        
        printf("soy el planificador\n");
        
        //politica_fcfs();
        politica_round_robin();
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

    if (pcb == NULL) {
        printf("No READY process found in the queue\n");
        return;
    }

    //si hay espacio en exec_queue ponerlo en exec_queue
    for(int i = 0; i < cpu.thread_num; i++){
        if(exec_queue[i] == NULL){
            exec_queue[i] = pcb; // Store pointer
            //ejecutar el proceso en un hilo
            pthread_create(&threads[i], NULL, proccess_execution_fcfs, exec_queue[i]);
            break;
        }
        printf("No hay espacio en exec_queue\n");
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
    sleep((int)pcb->completion_time);
    pcb->state = "FINISHED";
    printf("Proceso con PID: %d terminado\n", pcb->pid);
    return NULL;
}

void politica_round_robin(){
    //inicializa la lista de turnos y la lista de bloqueados
    if (block_threads == NULL){
        memset(listaDeTurnos, 0, sizeof(listaDeTurnos)); // Initialize with NULL
        listaDeTurnos[cpu.thread_num*2]; // Initialize with NULL
        block_threads = malloc(cpu.thread_num * sizeof(pthread_t));
        if (block_threads == NULL) {
            perror("Failed to allocate block_threads array");
            exit(EXIT_FAILURE);
        }
    }

    //insertar los procesos READY en la lista de turnos
    
    PCB* pcb = process_queue;
    while(pcb != NULL){
        if(strcmp(pcb->state, "READY") == 0){
            int already_in_list = 0;
            //verificar si el proceso ya esta en la lista de turnos
            for(int i = 0; i < cpu.thread_num*2; i++){
                if(listaDeTurnos[i] == pcb){
                    already_in_list = 1;
                    break;
                }
            }
            //si no esta en la lista de turnos, meterlo
            if (!already_in_list) {
                for(int i = 0; i < cpu.thread_num*2; i++){
                    if(listaDeTurnos[i] == NULL){
                        listaDeTurnos[i] = pcb;
                        break;
                    }
                }
            }
        }
        pcb = pcb->next;
    }
    for(int i = 0; i < cpu.thread_num*2; i++){
        if(listaDeTurnos[i] != NULL){
            printf("XXXXXXXXXXProceso con PID: %d en lista de turnos en la posicion %d\n", listaDeTurnos[i]->pid, i);
        }
    }

    //ejecutar la cantidad de procesos equivalentes a la cantidad de hilos de la lista de turnos (si estan en estado READY)
    for(int i = 0; i < cpu.thread_num; i++){
        if(listaDeTurnos[i] != NULL){
            //meter el proceso en exec_queue
            for(int j = 0; j < cpu.thread_num; j++){
                if(exec_queue[j] == NULL && strcmp(listaDeTurnos[i]->state, "READY") == 0){
                    exec_queue[j] = listaDeTurnos[i];
                    break;
                }
            }
            //ejecutar el proceso en un hilo
            pthread_create(&threads[i], NULL, proccess_execution_round_robin, listaDeTurnos[i]);
        }
    }
    
    //vaciar los procesos terminados o bloqueados de exec_queue
    for(int i = 0; i < cpu.thread_num; i++){
        if(exec_queue[i] != NULL && (strcmp(exec_queue[i]->state, "FINISHED") == 0 || strcmp(exec_queue[i]->state, "BLOCKED") == 0)){
            exec_queue[i] = NULL;
        }
    }

    //bloquear los procesos con estado BLOCKED
    for(int i = 0; i < cpu.thread_num*2; i++){
        if(listaDeTurnos[i] != NULL && strcmp(listaDeTurnos[i]->state, "BLOCKED") == 0){
            pthread_create(&block_threads[i], NULL, proccess_block, listaDeTurnos[i]);
        }
    }

    //vaciar los procesos terminados de la lista de turnos
    for(int i = 0; i < cpu.thread_num*2; i++){
        if(listaDeTurnos[i] != NULL && strcmp(listaDeTurnos[i]->state, "FINISHED") == 0){
            listaDeTurnos[i] = NULL;
        }
    }
}

void *proccess_execution_round_robin(void* arg){
    PCB* pcb = (PCB*)arg;
    pcb->state = "EXEC";
    printf("Proceso con PID: %d en ejecucion\n", pcb->pid);
    //ejecutar el tiempo de quantum o el tiempo restante del proceso. Si el proceso no termina, se bloquea (mas adelante esta la funcion de bloqueo)
    if(quantum < pcb->completion_time - pcb->execution_time){
        sleep(quantum);
        pcb->execution_time += quantum;
        pcb->state = "BLOCKED";
        printf("Proceso con PID: %d bloqueado\n", pcb->pid);
    } else {
        sleep(pcb->completion_time - pcb->execution_time);
        pcb->execution_time = pcb->completion_time;
        pcb->state = "FINISHED";
        printf("Proceso con PID: %d terminado\n", pcb->pid);
    }
}

void *proccess_block(void* arg){
    PCB* pcb = (PCB*)arg;
    pcb->state = "BLOCKED";
    sleep(3);
    pcb->state = "READY";
    printf("Proceso con PID: %d desbloqueado\n", pcb->pid);
}


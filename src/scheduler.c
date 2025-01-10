#include "../include/threads.h"
#include <stdio.h>
#include "../include/data_structures.h"
#include <unistd.h> 
#include <time.h>
#include <stdlib.h> // Added for malloc
#include <string.h> // Added for strcmp

extern pthread_cond_t cond_sched;
extern pthread_mutex_t queue_mutex;

int execution_threads_num = 0;

extern struct CPU
{
    int cpu_num;
    int core_num;
    int thread_num;
};

extern struct CPU cpu;

PCB** exec_queue = NULL; // Initialize to NULL
pthread_t *threads = NULL; // Initialize to NULL

void *proccess_execution_fcfs(void* arg); // Update function signature
void *proccess_execution_round_robin(void* arg); // Update function signature
void *proccess_block(void* arg); // Update function signature

//Variables para round robin
int quantum = 3;
PCB** listaDeTurnos = NULL;
pthread_t *block_threads;
int last_process = 0;
extern char* scheduler_type;

void *scheduler_thread(void* arg) {
    //actualiza el tamaño de exec_queue a la cantidad de hilos

    execution_threads_num = cpu.thread_num * cpu.core_num * cpu.cpu_num; // Update execution_threads_num

    exec_queue = realloc(exec_queue, execution_threads_num * sizeof(PCB*)); // Adjust exec_queue size
    if (exec_queue == NULL) {
        perror("Failed to allocate exec_queue array");
        exit(EXIT_FAILURE);
    }
    memset(exec_queue, 0, execution_threads_num * sizeof(PCB*)); // Initialize with NULL

    listaDeTurnos = realloc(listaDeTurnos, execution_threads_num * 2 * sizeof(PCB*)); // Adjust listaDeTurnos size
    if (listaDeTurnos == NULL) {
        perror("Failed to allocate listaDeTurnos array");
        exit(EXIT_FAILURE);
    }
    memset(listaDeTurnos, 0, execution_threads_num * 2 * sizeof(PCB*)); // Initialize with NULL

    threads = realloc(threads, execution_threads_num * sizeof(pthread_t)); // Allocate threads array
    if (threads == NULL) {
        perror("Failed to allocate threads array");
        exit(EXIT_FAILURE);
    }

    //crear tantos hilos como thread_num
    while(1){
        pthread_mutex_lock(&queue_mutex);
        pthread_cond_wait(&cond_sched, &queue_mutex); // Espera señal del timer 2 
        //printf("soy el planificador\n");
        
        if(strcmp(scheduler_type, "FCFS") == 0){
            politica_fcfs();
        } else {
            politica_round_robin();
        }

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
    for(int i = 0; i < execution_threads_num; i++){
        if(exec_queue[i] == NULL){
            exec_queue[i] = pcb; // Store pointer
            //ejecutar el proceso en un hilo
            pthread_create(&threads[i], NULL, proccess_execution_fcfs, exec_queue[i]);
            break;
        }
        printf("No hay espacio en exec_queue\n");
    }

    //vaciar los procesos terminados de exec_queue
    for(int i = 0; i < execution_threads_num; i++){
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
        block_threads = malloc(execution_threads_num * sizeof(pthread_t));
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
            for(int i = 0; i < execution_threads_num * 2; i++){
                if(listaDeTurnos[i] == pcb){
                    already_in_list = 1;
                    break;
                }
            }
            //si no esta en la lista de turnos, meterlo
            if (!already_in_list) {
                for(int i = 0; i < execution_threads_num * 2; i++){
                    if(listaDeTurnos[i] == NULL){
                        listaDeTurnos[i] = pcb;
                        break;
                    }
                }
            }
        }
        pcb = pcb->next;
    }
    for(int i = 0; i < execution_threads_num * 2; i++){
        if(listaDeTurnos[i] != NULL){
            printf("XXXXXXXXXXProceso con PID: %d en lista de turnos en la posicion %d\n", listaDeTurnos[i]->pid, i);
        }
    }

    //si el ultimo proceso que se metio en exec_queue ya termino, se reinicia el contador de procesos
    if(last_process >= execution_threads_num * 2){
        last_process = 0;
    }

    //ejecutar los procesos en la lista de turnos (que esten READY), metiendolos en exec_queue y quedandonos con la posicion de el ultimo proceso que hemos metido
    //para ejecutarlo en la siguiente vuelta
    for(int i = last_process; i < execution_threads_num * 2; i++){
        if(listaDeTurnos[i] != NULL && strcmp(listaDeTurnos[i]->state, "READY") == 0){
            for(int j = 0; j < execution_threads_num; j++){
                if(exec_queue[j] == NULL){
                    exec_queue[j] = listaDeTurnos[i];
                    pthread_create(&threads[j], NULL, proccess_execution_round_robin, exec_queue[j]);
                    last_process = i + 1; // Update last_process to the next index
                    break;
                }
            }
            break; // Exit the loop after scheduling one process
        }
    }

    //bloquear los procesos con estado BLOCKED
    for(int i = 0; i < execution_threads_num * 2; i++){
        if(listaDeTurnos[i] != NULL && strcmp(listaDeTurnos[i]->state, "BLOCKED1") == 0){
            pthread_create(&block_threads[i], NULL, proccess_block, listaDeTurnos[i]);
        }
    }

    //vaciar los procesos terminados de la lista de turnos
    for(int i = 0; i < execution_threads_num * 2; i++){
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
        //BLOCKED1 Es un estado para hacer saber al programa que tiene que bloquear este proceso
        pcb->state = "BLOCKED1";
        printf("Proceso con PID: %d bloqueado\n", pcb->pid);
    } else {
        sleep(pcb->completion_time - pcb->execution_time);
        pcb->execution_time = pcb->completion_time;
        pcb->state = "FINISHED";
        printf("Proceso con PID: %d terminado\n", pcb->pid);
    }
    //sacar proceso bloqueado o terminado de exec_queue
    for(int i = 0; i < execution_threads_num; i++){
        if(exec_queue[i] == pcb){
            exec_queue[i] = NULL;
        }
    }
}

void *proccess_block(void* arg){
    PCB* pcb = (PCB*)arg;
    pcb->state = "BLOCKED2";
    sleep(3);
    pcb->state = "READY";
    printf("Proceso con PID: %d desbloqueado\n", pcb->pid);
}


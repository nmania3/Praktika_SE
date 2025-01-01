#include "../include/threads.h"
#include <stdio.h>
#include "../include/data_structures.h"
#include <pthread.h>
#include <unistd.h> // Para sleep
#include <stdlib.h> // Para malloc


// Correct process_queue declaration
PCB* process_queue = NULL;

extern pthread_cond_t cond_gen;
extern pthread_mutex_t queue_mutex;
extern int proccess_generator_frequency;

// Añadir un proceso a la cola
void add_to_queue(PCB* new_pcb) {
    if (process_queue == NULL) {
        process_queue = new_pcb;
    } else {
        PCB* temp = process_queue;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_pcb;
    }
}


//liberar la memoria / borrar la cola de procesos
void free_queue() {
    PCB* current = process_queue;
    PCB* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    process_queue = NULL;
}

void *proccess_generator_thread(void* arg) {
    int pid = 0;

    while (1) {
        pid++;
        pthread_mutex_lock(&queue_mutex);
        pthread_cond_wait(&cond_gen, &queue_mutex); // Espera señal del timer 1
        printf("Generando proceso con PID: %d\n", pid);
        
        PCB* new_pcb = (PCB*)malloc(sizeof(PCB));
        new_pcb->pid = pid;
        new_pcb->next = NULL;
        add_to_queue(new_pcb);

        printf("Proceso con PID: %d agregado a la cola\n", pid);
        
        pthread_mutex_unlock(&queue_mutex);
        sleep(proccess_generator_frequency);
    }

    // Free the memory allocated for the PCBs
    free_queue();
}

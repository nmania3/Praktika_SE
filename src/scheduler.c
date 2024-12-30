#include "../include/threads.h"
#include <stdio.h>
#include "../include/data_structures.h"
#include <unistd.h> 

void *scheduler_thread(void* arg) {
    sleep(1);
    printf("soy el planificador\n");

    //Meter el primer proceso en ejecucion
    //
    
}
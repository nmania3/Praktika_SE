#include "../include/threads.h"
#include <stdio.h>
#include "../include/data_structures.h"
void *scheduler_thread(void* arg) {
    sleep(1);
    printf("soy el planificador\n");
    //por ahora nada
}
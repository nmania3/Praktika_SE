#include "../include/threads.h"
#include <stdio.h>
#include "../include/data_structures.h"

void *clock_thread(void *arg) {
    while (1)
    {
        sleep(clock_frequency);
        printf("soy el reloj\n");
    }
    
    
}
#ifndef THREADS_H
#define THREADS_H

void* clock_thread(void* arg);
void* timer_thread1(void* arg);
void* timer_thread2(void* arg);
void* scheduler_thread(void* arg);
void* proccess_generator_thread(void* arg);

#endif // THREADS_H

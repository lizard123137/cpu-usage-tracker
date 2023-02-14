#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 10

typedef struct CPU_DATA {
    int cpu;
    unsigned long user,
                  nice,
                  system,
                  idle,
                  iowait,
                  irq,
                  softirq,
                  steal,
                  guest,
                  guest_nice;
} CPU_DATA;

extern int onlineProcessorsAmount;

extern pthread_mutex_t readBufferMutex;
extern sem_t readSemaphoreEmpty;
extern sem_t readSemaphoreFull;

extern pthread_mutex_t printBufferMutex;
extern sem_t printSemaphoreEmpty;
extern sem_t printSemaphoreFull;

CPU_DATA *get_readbuffer_data(void);
unsigned long *get_printbuffer_data(void);
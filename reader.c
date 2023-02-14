#include "reader.h"

static int get_cpu_data(CPU_DATA *readbuffer_data) {
    FILE *file = fopen("/proc/stat", "r");

    char data[1024];

    if (file == NULL)
        return -1;

    for(int idx = 0; idx < onlineProcessorsAmount; idx++) {
        fgets(data, sizeof(data), file);

        sscanf(data, "cpu%d %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",  
            &readbuffer_data[idx].cpu,
            &readbuffer_data[idx].user,
            &readbuffer_data[idx].nice,
            &readbuffer_data[idx].system,
            &readbuffer_data[idx].idle,
            &readbuffer_data[idx].iowait,
            &readbuffer_data[idx].irq,
            &readbuffer_data[idx].softirq,
            &readbuffer_data[idx].steal,
            &readbuffer_data[idx].guest,
            &readbuffer_data[idx].guest_nice);
    }

    if (fclose(file) != 0)
        return -1;
    return 0;
}

void *reader(void *arg) {
    (void)arg;

    while(1) {
        sem_wait(&readSemaphoreFull);
        pthread_mutex_lock(&readBufferMutex);

        if (get_cpu_data(get_readbuffer_data()) == -1)
            exit(EXIT_FAILURE);
        
        pthread_mutex_unlock(&readBufferMutex);
        sem_post(&readSemaphoreEmpty);
        sleep(READ_INTERVAL);
    }
}
#include "reader.h"

void *reader(void *arg) {
    (void)arg;

    while(1) {
        sem_wait(&readSemaphoreFull);
        pthread_mutex_lock(&readBufferMutex);

        FILE *file = fopen("/proc/stat", "r");
        CPU_DATA *readbuffer_data = get_readbuffer_data();

        char data[1024];

        if (file == NULL)
            exit(EXIT_FAILURE);

        for(int32_t idx = 0; idx < onlineProcessorsAmount; idx++) {
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
            exit(EXIT_FAILURE);
        
        pthread_mutex_unlock(&readBufferMutex);
        sem_post(&readSemaphoreEmpty);
        sleep(READ_INTERVAL);
    }
}
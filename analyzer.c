#include "analyzer.h"

// MAGIC STACKOVERFLOW FORMULA
static unsigned long cpu_usage_percentage(CPU_DATA prev, CPU_DATA cur) {
    unsigned long prevIdle = prev.idle + prev.iowait;
    unsigned long idle = cur.idle + cur.iowait;

    unsigned long prevNonIdle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    unsigned long nonIdle = cur.user + cur.nice + cur.system + cur.irq + cur.softirq + cur.steal;

    unsigned long prevTotal = prevIdle + prevNonIdle;
    unsigned long total = idle + nonIdle;

    unsigned long totald = total - prevTotal;
    unsigned long idled = idle - prevIdle;

    return (totald - idled) * 100 / totald;
}

void *analyzer(void *arg) {
    (void)arg;

    CPU_DATA *prev;
    CPU_DATA *cur;

    unsigned long *processedData;
    unsigned long *printBufferPtr;

    if ((prev = malloc(sizeof(CPU_DATA) * (unsigned long)onlineProcessorsAmount)) == NULL)
        return NULL;

    if ((processedData = malloc(sizeof(unsigned long) * (unsigned long)onlineProcessorsAmount)) == NULL) {
        free(prev);
        return NULL;
    }

    pthread_cleanup_push(free, prev);
    pthread_cleanup_push(free, processedData);

    while(1) {
        sem_wait(&readSemaphoreEmpty);
        pthread_mutex_lock(&readBufferMutex);
    
        cur = get_readbuffer_data();
        for (int idx = 0; idx < onlineProcessorsAmount; idx++) {
            processedData[idx] = cpu_usage_percentage(prev[idx], cur[idx]);
            prev[idx] = cur[idx];
        }

        pthread_mutex_unlock(&readBufferMutex);
        sem_post(&readSemaphoreFull);

        sem_wait(&printSemaphoreEmpty);
        pthread_mutex_lock(&printBufferMutex);

        printBufferPtr = get_printbuffer_data();
        memcpy(printBufferPtr, processedData, (unsigned long)onlineProcessorsAmount * sizeof(unsigned long));

        pthread_mutex_unlock(&printBufferMutex);
        sem_post(&printSemaphoreFull);
    }

    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
}
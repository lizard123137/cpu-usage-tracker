#include "analyzer.h"

// MAGIC STACKOVERFLOW FORMULA
static uint64_t cpu_usage_percentage(CPU_DATA prev, CPU_DATA cur) {
    uint64_t prevIdle = prev.idle + prev.iowait;
    uint64_t idle = cur.idle + cur.iowait;

    uint64_t prevNonIdle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    uint64_t nonIdle = cur.user + cur.nice + cur.system + cur.irq + cur.softirq + cur.steal;

    uint64_t prevTotal = prevIdle + prevNonIdle;
    uint64_t total = idle + nonIdle;

    uint64_t totald = total - prevTotal;
    uint64_t idled = idle - prevIdle;

    return (totald - idled) * 100 / totald;
}

void *analyzer(void *arg) {
    (void)arg;

    CPU_DATA *prev;
    CPU_DATA *cur;

    uint64_t *processedData;
    uint64_t *printBufferPtr;

    if ((prev = malloc(sizeof(CPU_DATA) * (uint64_t)onlineProcessorsAmount)) == NULL)
        return NULL;

    if ((processedData = malloc(sizeof(uint64_t) * (uint64_t)onlineProcessorsAmount)) == NULL) {
        free(prev);
        return NULL;
    }

    pthread_cleanup_push(free, prev);
    pthread_cleanup_push(free, processedData);

    while(1) {
        sem_wait(&readSemaphoreEmpty);
        pthread_mutex_lock(&readBufferMutex);
    
        cur = get_readbuffer_data();
        for (int32_t idx = 0; idx < onlineProcessorsAmount; idx++) {
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
#include "printer.h"

void *printer (void *arg) {
    (void)arg;

    while(1) {
        sem_wait(&printSemaphoreFull);
        pthread_mutex_lock(&printBufferMutex);

        uint64_t *printBufferPtr = get_printbuffer_data();

        system("clear");

        printf("\033[30;47m CPU USAGE FOR EACH CORE \033[0m\n\n");
        for (int32_t idx = 1; idx < onlineProcessorsAmount; idx++) {
            
            printf("CPU CORE %d| %s%.*s\033[0m %*lu%%\n",
                idx,
                (printBufferPtr[idx] < WARNING_THRESHOLD) ? "\033[32;42m" : "\033[31;41m",
                (uint32_t)printBufferPtr[idx]/2,
                "                                                  ",
                (uint32_t)(50 - printBufferPtr[idx]/2),
                printBufferPtr[idx]);
        }

        pthread_mutex_unlock(&printBufferMutex);
        sem_post(&printSemaphoreEmpty);

        sleep(1);
    }
}
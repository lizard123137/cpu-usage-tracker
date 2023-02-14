#include "printer.h"

void *printer (void *arg) {
    (void)arg;

    while(1) {
        sem_wait(&printSemaphoreFull);
        pthread_mutex_lock(&printBufferMutex);

        unsigned long *printBufferPtr = get_printbuffer_data();

        system("clear");

        printf("\033[30;47m CPU USAGE FOR EACH CORE \033[0m\n\n");
        for (int idx = 1; idx < onlineProcessorsAmount; idx++) {
            
            printf("CPU CORE %d| %s%.*s\033[0m %*lu%%\n",
                idx,
                (printBufferPtr[idx] < WARNING_THRESHOLD) ? "\033[32;42m" : "\033[31;41m",
                (int)printBufferPtr[idx]/2,
                "                                                  ",
                (int)(50 - printBufferPtr[idx]/2),
                printBufferPtr[idx]);
        }

        pthread_mutex_unlock(&printBufferMutex);
        sem_post(&printSemaphoreEmpty);

        sleep(1);
    }
}
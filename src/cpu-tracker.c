#include "cpu-tracker.h"

#include "reader.h"
#include "printer.h"
#include "analyzer.h"

static CPU_DATA *readBuffer[BUFFER_SIZE];
pthread_mutex_t readBufferMutex = PTHREAD_MUTEX_INITIALIZER;
sem_t readSemaphoreEmpty;
sem_t readSemaphoreFull;

static uint64_t *printBuffer[BUFFER_SIZE];
pthread_mutex_t printBufferMutex = PTHREAD_MUTEX_INITIALIZER;
sem_t printSemaphoreEmpty;
sem_t printSemaphoreFull;

static uint8_t watchdogThreads[3] = { 0 };
pthread_mutex_t watchdogMutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_t reader_thread;
static pthread_t printer_thread;
static pthread_t analyzer_thread;

uint8_t onlineProcessorsAmount = 0;
uint8_t running = 1;

CPU_DATA *get_readbuffer_data(void) {
    int32_t idx;
    sem_getvalue(&readSemaphoreEmpty, &idx);
    return readBuffer[idx];
}

uint64_t *get_printbuffer_data(void) {
    int32_t idx;
    sem_getvalue(&printSemaphoreEmpty, &idx);
    return printBuffer[idx];
}

static void handle_sigactions(int32_t sig) {
    (void)sig;
    printf("\n CLOSING THREADS...\n");
    running = 0;

    pthread_cancel(reader_thread);
    pthread_cancel(analyzer_thread);
    pthread_cancel(printer_thread);
}

static void init_buffers(void) {
    for (int32_t idx = 0; idx < BUFFER_SIZE; idx++) {
        readBuffer[idx] = malloc(sizeof(CPU_DATA) * (uint64_t)onlineProcessorsAmount);
        printBuffer[idx] = malloc(sizeof(uint64_t) * (uint64_t)onlineProcessorsAmount);

        if(readBuffer[idx] == NULL || printBuffer[idx] == NULL)
            exit(EXIT_FAILURE);
    }
}

static void init_semaphores(void) {
    if (sem_init(&readSemaphoreEmpty, 0, 0))
        exit(EXIT_FAILURE);
    if (sem_init(&readSemaphoreFull, 0, BUFFER_SIZE))
        exit(EXIT_FAILURE);
    
    if (sem_init(&printSemaphoreEmpty, 0, 0))
        exit(EXIT_FAILURE);
    if (sem_init(&printSemaphoreFull, 0, BUFFER_SIZE))
        exit(EXIT_FAILURE);
}

static void dealocate_data(void) {
    pthread_mutex_destroy(&readBufferMutex);
    sem_destroy(&readSemaphoreEmpty);
    sem_destroy(&readSemaphoreFull);

    pthread_mutex_destroy(&printBufferMutex);
    sem_destroy(&printSemaphoreEmpty);
    sem_destroy(&printSemaphoreFull);

    for (int32_t idx = 0; idx < BUFFER_SIZE; idx++) {
        free(readBuffer[idx]);
        free(printBuffer[idx]);
    }
}

void inform_watchdog(uint8_t idx) {
    pthread_mutex_lock(&watchdogMutex);
    watchdogThreads[idx] = 1;
    pthread_mutex_unlock(&watchdogMutex);
}

static void watchdog(void) {
    uint64_t timesSinceLastUpdate[3];
    
    for (int32_t idx = 0; idx < 3; idx++) {
        timesSinceLastUpdate[idx] = time(0);
    }

    while(running) {
        sleep(WATCHDOG_TIMEOUT);

        pthread_mutex_lock(&watchdogMutex);
        for (int32_t idx = 0; idx < 3; idx++) {
            if(watchdogThreads[idx] == 1)
                timesSinceLastUpdate[idx] = time(0);

            if(time(0) - timesSinceLastUpdate[idx] > 2) {
                running = 0;
                handle_sigactions(-1);
                return;
            }
                
            watchdogThreads[idx] = 0;
        }
        pthread_mutex_unlock(&watchdogMutex);
    }
}

static void init_watchdog(void) {
    for (int32_t idx = 0; idx < 3; idx++) {
        watchdogThreads[idx] = 0;
    }
}

int32_t main(void) {
    struct sigaction sa;
    sa.sa_handler = &handle_sigactions;
    sa.sa_flags = SA_INTERRUPT;

    onlineProcessorsAmount = (uint32_t)sysconf(_SC_NPROCESSORS_ONLN) + 1;

    init_buffers();
    init_semaphores();
    init_watchdog();

    pthread_create(&reader_thread, NULL, reader, NULL);
    pthread_create(&analyzer_thread, NULL, analyzer, NULL);
    pthread_create(&printer_thread, NULL, printer, NULL);

    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    watchdog();

    pthread_join(reader_thread, NULL);
    pthread_join(analyzer_thread, NULL);
    pthread_join(printer_thread, NULL);

    dealocate_data();    
    return 0;
}
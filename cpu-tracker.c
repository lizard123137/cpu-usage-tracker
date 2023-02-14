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

    pthread_cancel(reader_thread);
    pthread_cancel(analyzer_thread);
    pthread_cancel(printer_thread);
    running = 0;
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

    for (int32_t i = 0; i < BUFFER_SIZE; i++) {
        free(readBuffer[i]);
        free(printBuffer[i]);
    }
}

static void watchdog(void) {
    while(running) {
        sleep(1);   // TODO: everything :(
    }
}

int32_t main(void) {
    struct sigaction sa;
    sa.sa_handler = &handle_sigactions;
    sa.sa_flags = SA_INTERRUPT;

    onlineProcessorsAmount = (uint32_t)sysconf(_SC_NPROCESSORS_ONLN) + 1;

    init_buffers();
    init_semaphores();

    pthread_create(&reader_thread, NULL, reader, NULL);
    pthread_create(&analyzer_thread, NULL, analyzer, NULL);
    pthread_create(&printer_thread, NULL, printer, NULL);

    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    pthread_join(reader_thread, NULL);
    pthread_join(analyzer_thread, NULL);
    pthread_join(printer_thread, NULL);

    watchdog();

    dealocate_data();
    return 0;
}
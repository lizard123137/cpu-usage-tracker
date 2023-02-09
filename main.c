#include <stdio.h>
#include <pthread.h>

void *Printer();

int main() {
    pthread_t printer_thread;

    pthread_create(&printer_thread, NULL, computation, NULL);
    pthread_join(printer_thread, NULL);

    return 0;
}

void *Printer() {
    printf("Example\n");
    return NULL;
}
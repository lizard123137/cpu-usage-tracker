#include "cpu-tracker.h"
#include "printer.h"

static pthread_t printer_thread;

int main() {
    pthread_create(&printer_thread, NULL, printer, NULL);
    pthread_join(printer_thread, NULL);

    return 0;
}
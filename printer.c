#include "printer.h"

void *printer (void *arg) {
    while(1) {
        system("clear");
        printf("Test");
        
        sleep(1);
    }
}
#include <stdio.h>
#include "pico/stdlib.h"

void connectusb();

int main() {
    
    
 
    while (1) {
        char message[100];
        scanf("%s", message);
        printf("message: %s\r\n",message);
        sleep_ms(50);
    }
}

void connectusb(){
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Start!\n");
}
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

void connectusb();

int main() {

    // initializing button, led, adc
    gpio_init(0);               // initializing button
    gpio_set_dir(0,GPIO_IN);
    gpio_init(1);               // initializing LED
    gpio_set_dir(1,GPIO_OUT);
    
    adc_init();                 // init the adc module
    adc_gpio_init(26);          // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0);        // select to read from ADC0
    
    // --------------------------------//
    connectusb();       // enable usb communication and wait until open

}

void connectusb(){
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Start!\n");
}
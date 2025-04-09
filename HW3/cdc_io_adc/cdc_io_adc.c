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

    gpio_put(1,1);      // turn on LED
    while (gpio_get(0) == 1){   // Wait until button is pushed
        ;
    }
    gpio_put(1,0);      // turn off LED

    while (1){
        int samples;
        printf("Enter a number of samples: \r\n");
        scanf("%d", &samples);

        for (int i = 0; i < samples; i++){
            uint16_t result = adc_read();   // Max is 4095 = 3.3 V
            float volts = (result * 3.3) / (1 << 12);
            printf("    %.2f\n",volts);
            sleep_ms(10);       // 100 Hz
        }
    }
}

void connectusb(){
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Connected!\n");
}


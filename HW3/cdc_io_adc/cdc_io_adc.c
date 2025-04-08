#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

void connectusb();

int main() {
    
    
}

void connectusb(){
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Start!\n");
}

// gpio_init(0);       // initializing pin GP0 (pin 1 on pi)
//     gpio_init(1);       // initialization pin GP1 (pin 2 on pi)

//     gpio_set_dir(0,GPIO_IN);    // for a button input, default state is 1
//     gpio_set_dir(1,GPIO_OUT);   // for an LED output
 
//     while (1) {
//         if (gpio_get(0) == 0){      // if the button is pressed
//             gpio_put(1,1);          // turn led on
//             sleep_ms(100);          // wait a moment
//         }
//         gpio_put(1,0);              // LED is off otherwise
//     }
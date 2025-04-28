#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define LEDPin 25 // the built in LED on the Pico

int main(){
    stdio_init_all();

    gpio_init(LEDPin);

    gpio_set_function(LEDPin, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(LEDPin); // Get PWM slice number
    float div = 3; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = 50000; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM

    while (true){
        pwm_set_gpio_level(LEDPin, wrap / 1);
        sleep_ms(1000);
        pwm_set_gpio_level(LEDPin, wrap / 2);
        sleep_ms(1000);
        pwm_set_gpio_level(LEDPin, wrap / 3);
        sleep_ms(1000);
        pwm_set_gpio_level(LEDPin, wrap / 4);
        sleep_ms(1000);
    }
}


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define PHASE 18
#define ENABLE 19
#define TOP 65535

void connectusb();
void pwmsetup();

int main()
{
    stdio_init_all();

    gpio_init(PHASE);   // Initializing the forward/reverse pin
    gpio_set_dir(PHASE, GPIO_OUT);

    gpio_init(ENABLE);  // Initializing the PWM pin
    gpio_set_function(ENABLE, GPIO_FUNC_PWM);

    connectusb();
    pwmsetup();

    while (true) {
        gpio_put(PHASE, 1);
        pwm_set_gpio_level(ENABLE, TOP);
        sleep_ms(2000);
        gpio_put(PHASE, 0);
        pwm_set_gpio_level(ENABLE, TOP);
        sleep_ms(20000);
    }
}

void connectusb(){
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Connected!\n");
}

void pwmsetup(){
    uint slice_num = pwm_gpio_to_slice_num(ENABLE);
    // Configure PWM: 100% duty cycle, default frequency (~1kHz is fine)
    int pwm_config = pwm_get_default_config();
    pwm_init(slice_num, &pwm_config, true);  // true = start PWM immediately

    pwm_set_wrap(slice_num, TOP);
}
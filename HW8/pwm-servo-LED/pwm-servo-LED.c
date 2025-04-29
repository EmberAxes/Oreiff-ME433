#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define LEDPin 25 // the built in LED on the Pico
#define SERVOPin 15

void connectusb();

int main(){
    connectusb();
    stdio_init_all();

    gpio_init(SERVOPin);
    gpio_set_function(SERVOPin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVOPin);
    float div = 60;
    pwm_set_clkdiv(slice_num, div);
    uint16_t wrap = 50000;
    pwm_set_wrap(slice_num,wrap);
    pwm_set_enabled(slice_num, true);

    float angle = 0;
    uint16_t setpwm;
    float sec = 2.;         // How long to go 180 degrees?
    float inc = 0.5;        // Angle increment
    float pause;

    while (true){

                
        // 0 - 180
        // 0.025 - 0.125   difference of 0.1
        
        while (angle<180){
            pause = (180./inc) / (1000*sec);
            setpwm = ((angle / 180.0)*0.1 + 0.025) * wrap;
            pwm_set_gpio_level(SERVOPin, setpwm);
            angle+=0.5;
            sleep_ms(pause);
        }
        
        while (angle>0){
            pause = (180./inc) / (1000*sec);
            setpwm = ((angle / 180.0)*0.1 + 0.025) * wrap;
            pwm_set_gpio_level(SERVOPin, setpwm);
            angle-=0.5;
            sleep_ms(pause);
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

/*
Can only move 0 - 180 degrees
3 wires: Brown = ground, red = voltage, orange = PWM. Use 5V from VBUS
Servo can only be between 2.5 and 12.5
Set PWM for 50000
Call function so servo moves from 0 to 180 and back in 4 seconds
Function: set angle, turn angle into PWM

Say what 16-bit number (65535) number to count up to
- Divisor is a float
- Counter: 150000000/(divisor)/(speed I want to go)
*/


/* PWM Test Success
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
*/

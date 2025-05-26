#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <math.h>

#define PHASE 18
#define ENABLE 19
#define TOP 65535
#define MAX_DUTY 1
#define MIN_DUTY 0.12

void connectusb();
void pwmsetup();
void setspeed(float pwm);

int main()
{
    stdio_init_all();

    gpio_init(PHASE);   // Initializing the forward/reverse pin
    gpio_set_dir(PHASE, GPIO_OUT);

    gpio_init(ENABLE);  // Initializing the PWM pin
    gpio_set_function(ENABLE, GPIO_FUNC_PWM);

    connectusb();
    pwmsetup();
    gpio_put(PHASE, 1);
    pwm_set_gpio_level(ENABLE, 0);

    float duty = 0;
    float newduty = 0;

    while (true) {
        char change;
        printf("+ to increase, - to decrease\n");
        scanf("%c", &change);

        if (change == '+'){duty+=0.01;}    // increase speed
        if (change == '-'){duty-=0.01;}    // decrease speed

        newduty = duty;

        if (duty > 0){                     // run forward or zero          
            if (duty > MAX_DUTY){newduty = MAX_DUTY;}        // prevent above 100% duty cycle
            if (duty < MIN_DUTY){newduty = MIN_DUTY;}    // motor doesn't move below 10
            gpio_put(PHASE, 1);
            setspeed(newduty);
            printf("Forward duty cycle: %.3f\n",duty);
        }
        if (duty < 0){                              // run backward
            if (duty < -MAX_DUTY){newduty = -MAX_DUTY;}        // prevent above 100% duty cycle
            if (duty > -MIN_DUTY){newduty = -MIN_DUTY;}    // motor doesn't move below 10
            gpio_put(PHASE, 0);
            setspeed(fabs(newduty));
            printf("Backward duty cycle: %.3f\n",duty);
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

void pwmsetup(){
    uint slice_num = pwm_gpio_to_slice_num(ENABLE);
    // Configure PWM: 100% duty cycle, default frequency (~1kHz is fine)
    pwm_config config = pwm_get_default_config();
    pwm_init(slice_num, &config, true);  // true = start PWM immediately

    pwm_set_wrap(slice_num, TOP);
}

void setspeed(float pwm){
    int sp = (int)round(pwm*TOP);
    pwm_set_gpio_level(ENABLE, sp);
}
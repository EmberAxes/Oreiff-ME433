#include <stdio.h>
#include "pico/stdlib.h"
#include "cam.h"
#include <math.h>

#define PHASEL 18   // A
#define ENABLEL 19
#define PHASER 20   // B
#define ENABLER 21
#define TOP 65535
#define MAX_DUTY 1
#define MIN_DUTY 0.12

void connectusb();   // will be commented out
void pwmsetupA();
void pwmsetupB();
void setspeed(float pwm, int enable);
void phaseenablesetup();

int main()
{
    stdio_init_all();
    connectusb();
    init_camera_pins();
    phaseenablesetup();
    pwmsetupA();
    pwmsetupB();

    float duty = 0;
    float newduty = 0;

    while (true) {
        setSaveImage(1);
        while(getSaveImage()==1){}
        convertImage();
        int com = findLine(IMAGESIZEY/2); // calculate the position of the center of the line
        // Congrats now I have found the center of the line

        // Go straight test portion
        if (com < 45 & com > 35){
            newduty = 0.5;
            // Both motors go forward
            gpio_put(PHASEL, 1);
            gpio_put(PHASER, 0);
            setspeed(newduty, ENABLEL);
            setspeed(newduty, ENABLER);
        }else{
            setspeed(0, ENABLEL); // STOP
            setspeed(0, ENABLER); // STOP
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

void pwmsetupA(){
    uint slice_num = pwm_gpio_to_slice_num(ENABLEL);
    // Configure PWM: 100% duty cycle, default frequency (~1kHz is fine)
    pwm_config config = pwm_get_default_config();
    pwm_init(slice_num, &config, true);  // true = start PWM immediately

    pwm_set_wrap(slice_num, TOP);
}

void pwmsetupB(){
    uint slice_num = pwm_gpio_to_slice_num(ENABLER);
    // Configure PWM: 100% duty cycle, default frequency (~1kHz is fine)
    pwm_config config = pwm_get_default_config();
    pwm_init(slice_num, &config, true);  // true = start PWM immediately

    pwm_set_wrap(slice_num, TOP);
}

void setspeed(float pwm, int enable){
    int sp = (int)round(pwm*TOP);
    pwm_set_gpio_level(enable, sp);
}

void phaseenablesetup(){
    gpio_init(PHASEL);   // Initializing the forward/reverse pin
    gpio_set_dir(PHASEL, GPIO_OUT);
    gpio_init(PHASER);   // Initializing the forward/reverse pin
    gpio_set_dir(PHASER, GPIO_OUT);

    gpio_init(ENABLEL);  // Initializing the PWM pin
    gpio_set_function(ENABLEL, GPIO_FUNC_PWM);
    gpio_init(ENABLER);  // Initializing the PWM pin
    gpio_set_function(ENABLER, GPIO_FUNC_PWM);

    pwmsetupA();
    pwmsetupB();

    gpio_put(PHASEL, 1);
    gpio_put(PHASER, 1);
    pwm_set_gpio_level(ENABLEL, 0);
    pwm_set_gpio_level(ENABLER, 0);
}

// image thing for later
// while (true) {
//         setSaveImage(1);
//         while(getSaveImage()==1){}
//         convertImage();
//         int com = findLine(IMAGESIZEY/2); // calculate the position of the center of the line
//     }

// Old make motors run manually code
//         char change;
//         printf("+ to increase, - to decrease\n");
//         scanf("%c", &change);

//         if (change == '+'){duty+=0.01;}    // increase speed
//         if (change == '-'){duty-=0.01;}    // decrease speed

//         newduty = duty;

//         if (duty > 0){                     // run forward or zero          
//             if (duty > MAX_DUTY){newduty = MAX_DUTY;}        // prevent above 100% duty cycle
//             if (duty < MIN_DUTY){newduty = MIN_DUTY;}    // motor doesn't move below 10
//             gpio_put(PHASEL, 1);
//             gpio_put(PHASER, 0);
//             setspeed(newduty, ENABLEL);
//             setspeed(newduty, ENABLER);
//             printf("Forward duty cycle: %.3f\n",duty);
//         }
//         if (duty < 0){                              // run backward
//             if (duty < -MAX_DUTY){newduty = -MAX_DUTY;}        // prevent above 100% duty cycle
//             if (duty > -MIN_DUTY){newduty = -MIN_DUTY;}    // motor doesn't move below 10
//             gpio_put(PHASEL, 0);
//             gpio_put(PHASER, 1);
//             setspeed(fabs(newduty),ENABLEL);
//             setspeed(fabs(newduty),ENABLER);
//             printf("Backward duty cycle: %.3f\n",duty);
//         }
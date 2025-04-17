#include <stdio.h>
#include "pico/stdlib.h"

void connectusb();

int main()
{
    connectusb();
    stdio_init_all();

    volatile float f1, f2;
    printf("Enter two floats to use:\r\n");
    scanf("%f %f", &f1, &f2);
    volatile float f_add, f_sub, f_mult, f_div;

    // ADDITION
    absolute_time_t t1 = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_add = f1 + f2;
    }
    float t_dif = to_us_since_boot(t1);
    float t_per_calc = (float)t_dif / 1000;
    float cycles = t_per_calc / 0.006667;

    while (1){
        printf("Cycles = %f\r\n", cycles);
        sleep_ms(1000);
    }
    
    // f_sub = f1-f2;
    // f_mult = f1*f2;
    // f_div = f1/f2;
    // printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);
}

void connectusb(){
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Connected!\n");
}
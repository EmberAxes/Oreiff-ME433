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
    printf("Float 1 %f, float 2 %f\r\n",f1, f2);

    volatile float f_add, f_sub, f_mult, f_div;

    // ADDITION ----------------------------------------------
    absolute_time_t t1 = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_add = f1 + f2;
    }
    uint64_t t = to_us_since_boot(t1)/1000;
    uint64_t cycles = t/667;
    printf("Sum = %f\r\n",f_add);
    printf("Cycles = %llu\n", cycles);

    // Subtraction -------------------------------------------
    absolute_time_t t1 = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_sub = f1 - f2;
    }
    uint64_t t = to_us_since_boot(t1)/1000;
    uint64_t cycles = t/667;
    printf("Difference = %f\r\n",f_sub);
    printf("Cycles = %llu\n", cycles);

    // Multiplication -----------------------------------------
    absolute_time_t t1 = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_mult = f1 * f2;
    }
    uint64_t t = to_us_since_boot(t1)/1000;
    uint64_t cycles = t/667;
    printf("Product = %f\r\n",f_mult);
    printf("Cycles = %llu\n", cycles);

    // Division ------------------------------------------------
    absolute_time_t t1 = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_div = f1 / f2;
    }
    uint64_t t = to_us_since_boot(t1)/1000;
    uint64_t cycles = t/667;
    printf("Product = %f\r\n",f_div);
    printf("Cycles = %llu\n", cycles);
    
}

void connectusb(){
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Connected!\n");
}
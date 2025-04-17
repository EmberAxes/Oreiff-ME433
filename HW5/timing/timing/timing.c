#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();

    volatile float f1, f2;
    printf("Enter two floats to use:");
    scanf("%f %f", &f1, &f2);
    volatile float f_add, f_sub, f_mult, f_div;

    absolute_time_t t1 = get_absolute_time();  // start time
    for (int i = 0; i < 1000; i++){
        f_add = f1+f2;
    }
    uint64_t t_dif = to_us_since_boot(t1);
    uint64_t t_per_calc = t_dif/1000;    // Time in microseconds
    float cycles = t_per_calc/0.006667;
    printf("Cycles = %f\r\n", cycles);
    
    // f_sub = f1-f2;
    // f_mult = f1*f2;
    // f_div = f1/f2;
    // printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);
}

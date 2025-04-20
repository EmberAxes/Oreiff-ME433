#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

void setPin(unsigned char addr, unsigned char reg, unsigned char val);
unsigned char readPin(unsigned char address, unsigned char register);

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // Heartbeat initialized
    gpio_init(25);
    gpio_set_dir(25,GPIO_OUT);

    // GP7 out and GP0 in initialization
    setPin(0, 0x00, 0b01111111);  // This should initialize
                                  // GP0 as input and GP7 as output

    while (true) {
        
        // Heartbeat
        gpio_put(25,1);
        sleep_ms(250);
        gpio_put(25,0);
        sleep_ms(250);
    }
}

void setPin(unsigned char addr, unsigned char reg, unsigned char val){
    unsigned char buff[2];
    unsigned char address = 0;
    address = (1 << 7) | (addr << 1);  // 0 1 0 0 a d r 0  The 0 means write
    buff[0] = reg;
    buff[1] = val;

    gpio_put(I2C_SCL, 0);       // Set SCL low
    i2c_write_blocking(i2c_default, addr, buff, 2, false);
    gpio_put(I2C_SCL, 1);       // set back to high
}
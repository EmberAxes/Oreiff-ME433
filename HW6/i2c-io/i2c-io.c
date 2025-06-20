#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

void setPin(unsigned char addr, unsigned char reg, unsigned char val);
uint8_t readPin(unsigned char addr, uint8_t reg);

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
        if (readPin(0, 0x09)){
            setPin(0, 0x0A, 0b10000000);
        }else{
            setPin(0, 0x0A, 0b00000000);
        };
        sleep_ms(250);

        gpio_put(25,0);
        if (readPin(0, 0x09)){
            setPin(0, 0x0A, 0b10000000);
        }else{
            setPin(0, 0x0A, 0b00000000);
        };
        sleep_ms(250);
    }
}

void setPin(unsigned char addr, unsigned char reg, unsigned char val){
    unsigned char buff[2];
    // unsigned char address;
    // address = 0x20;   // base address
    addr = 0x20 | (addr & 0b111);  // 0 1 0 0 a d r 0  The 0 means write
    buff[0] = reg;
    buff[1] = val;

    i2c_write_blocking(i2c_default, addr, buff, 2, false);
}

uint8_t readPin(unsigned char addr, uint8_t reg){
    addr = 0x20 | (addr & 0b111);
    uint8_t regist[1];

    regist[0] = reg;
    i2c_write_blocking(i2c_default, addr, regist, 1, true);
    
    uint8_t buff[1];
    i2c_read_blocking(i2c_default, addr, buff, 1, false);

    buff[0] = buff[0] & 0b00000001;
    return(buff[0]);
}

//setPin(0, 0x0A, 0b10000000);    // Turn GP7 on
//setPin(0, 0x0A, 0b00000000);    // Turn GP7 off
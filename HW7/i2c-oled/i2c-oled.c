#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

void drawLetter(int x, int y, char c);
void drawmessage(int x, int y, char *m);

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Heartbeat initialized
    gpio_init(25);
    gpio_set_dir(25,GPIO_OUT);

    // display setup
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    while (true) {
        gpio_put(25,1);
        ssd1306_drawPixel(64,16,1);
        ssd1306_update();
        sleep_ms(1000);

        gpio_put(25,0);
        ssd1306_drawPixel(64,16,0);
        ssd1306_update();
        sleep_ms(1000);
    }
}

void drawLetter(int x, int y, char c){
    int j;
    for (j=0;j<5;j++){      // Every ascii character is 5 columns
        char col = ASCII[c-0x20][0];

        int i;
        for (i=0;i<8;i++){  // Each column is 8 bits
            char bit = (col >> i)&0b1;      // Checking the bit status
            ssd1306_drawPixel(x+j, y+i, bit)
        }
    }
}
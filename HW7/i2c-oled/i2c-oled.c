#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
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

    // ADC initialized
    adc_init();                 // init the adc module
    adc_gpio_init(26);          // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0);        // select to read from ADC0

    // display setup
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    while (true) {

        uint16_t result = adc_read();   // Max is 4095 = 3.3 V
        float volts = (result * 3.3) / (1 << 12);
        char message[50]; 
        sprintf(message, "my var = %.3f", volts); 

        drawMessage(20,10,message); // draw starting at x=20,y=10  

        gpio_put(25,1);
        sleep_ms(1000);

        gpio_put(25,0);
        sleep_ms(1000);

        ssd1306_clear();
        ssd1306_update();

        
    }
}

void drawLetter(int x, int y, char c){
    int j;
    for (j=0;j<5;j++){      // Every ascii character is 5 columns
        char col = ASCII[c-0x20][j];

        int i;
        for (i=0;i<8;i++){  // Each column is 8 bits
            char bit = (col >> i)&0b1;      // Checking the bit status
            ssd1306_drawPixel(x+j, y+i, bit);
        }
    }
}

void drawMessage(int x, int y, char *m){
    int i = 0;   // Start at the first letter
    while(m[i] != 0){       // While it is NOT ASCII null
        drawLetter(x+i*6, y, m[i]);
        i++;
    }
}
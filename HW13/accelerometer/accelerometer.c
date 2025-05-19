#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define ACC_PORT i2c0
#define ACC_SDA 16
#define ACC_SCL 17
#define OLED_PORT i2c1
#define OLED_SDA 18
#define OLED_SCL 19
// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75


void drawLetter(int x, int y, char c);
void drawMessage(int x, int y, char *m);

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

    char message[50], fps[30];


    while (true) {

        ssd1306_clear();                             // clear display
        sprintf(message, "Test");   // put volts into string
        drawMessage(20,10,message);                  // display string
        ssd1306_update();                            // update screen
    
        gpio_put(25,1);
        sleep_ms(250);
        
        //-----------------------------------------------------------------------
        ssd1306_clear();                             // clear display
        sprintf(message, "Ing");   // put volts into string
        drawMessage(20,10,message);                  // display string
        ssd1306_update();                            // update screen
    
        gpio_put(25,1);
        sleep_ms(250);
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

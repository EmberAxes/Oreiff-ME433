#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
#define I2C_PORT i2c0
#define I2C_SDA 16
#define I2C_SCL 17
#define ADDR_IMU 0x68
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
void acc_init();
void i2c_write(unsigned char addr, unsigned char reg, unsigned char val);
uint8_t i2c_read(unsigned char addr, uint8_t reg);

int main()
{
    stdio_init_all();

    //I2C Initialisation. Using it at 400Khz.
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

    // accelerometer setup
    acc_init();
    char message[50];
    float test;

    while (true) {

        ssd1306_clear();                      
        test = i2c_read(ADDR_IMU, WHO_AM_I);  
        sprintf(message, "Test %f", test);   
        drawMessage(20,10,message);                  // display string
        ssd1306_update();                            // update screen
        gpio_put(25,1);
        sleep_ms(250);

        //-----------------------------------------------------------------------
        ssd1306_clear();                             // clear display
        sprintf(message, "Ing %d", test+3);   // put volts into string
        drawMessage(20,10,message);                  // display string
        ssd1306_update();                            // update screen
    

        gpio_put(25,0);
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


void acc_init(){
    unsigned char buff[2];
    buff[0] = PWR_MGMT_1;
    buff[1] = 0x00;
    i2c_write_blocking(i2c_default,ADDR_IMU,buff,2,false);
}

uint8_t i2c_read(unsigned char addr, uint8_t reg){

    uint8_t regist[1];

    regist[0] = reg;
    i2c_write_blocking(i2c_default, addr, regist, 1, true);
    
    uint8_t buff[1];
    i2c_read_blocking(i2c_default, addr, buff, 1, false);

    buff[0] = buff[0] & 0b01111110;
    return(buff[0]);
}

void i2c_write(unsigned char addr, unsigned char reg, unsigned char val){
    unsigned char buff[2];

    addr = 0x68 | (addr & 0b111);  // 0 1 0 0 a d r 0  The 0 means write
    buff[0] = reg;
    buff[1] = val;

    i2c_write_blocking(i2c_default, addr, buff, 2, false);
}
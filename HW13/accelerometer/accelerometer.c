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
float i2c_read(unsigned char addr, uint8_t reg, int axis);
int acc2pix(float acc, int axis);

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
    float X, Y;
    int x, y;
    gpio_put(25,1);

    while (true) {

        ssd1306_clear();                      
        X = i2c_read(ADDR_IMU, ACCEL_XOUT_H, 0);   // Read x and y
        Y = i2c_read(ADDR_IMU, ACCEL_XOUT_H, 1);
        
        x = acc2pix(X,0);                 // Turn acc to pixel
        y = acc2pix(Y,1);
        
        ssd1306_drawPixel(x, y, 1);            // write to oled
        ssd1306_update();                            // update screen
       
        sleep_ms(10);
    }
}

int acc2pix(float acc, int axis){
    // x axis = 0, y axis = 1
    int coords[2];
    coords[0] = 64*(1-acc);
    coords[1] = 16*(1+acc);

    if (coords[0] < 0){
        coords[0] = 0;
    }
    if (coords[0] > 128){
        coords[0] = 128;
    }
    if (coords[1] < 0){
        coords[1] = 0;
    }
    if (coords[1] > 32){
        coords[1] = 32;
    }

    return(coords[axis]);
}

void acc_init(){
    unsigned char buff[2]; // Turn on
    buff[0] = PWR_MGMT_1;
    buff[1] = 0x00;
    i2c_write_blocking(i2c_default,ADDR_IMU,buff,2,false);

    unsigned char buff2[2]; // enable accelerometer
    buff2[0] = ACCEL_CONFIG;
    buff2[1] = 0b11100000;
    i2c_write_blocking(i2c_default,ADDR_IMU,buff2,2,false);
}

float i2c_read(unsigned char addr, uint8_t reg, int axis){
    /*
    X: axis = 0
    Y: axis = 1
    Z: axis = 2
    */

    uint8_t regist[1];

    regist[0] = reg;
    i2c_write_blocking(i2c_default, addr, regist, 1, true);
    
    uint8_t buff[6];
    i2c_read_blocking(i2c_default, addr, buff, 6, false);

    int16_t x = (buff[0] << 8 ) | buff[1];
    int16_t y = (buff[2] << 8 ) | buff[3];
    int16_t z = (buff[4] << 8 ) | buff[5];

    float coord[3];
    coord[0] = x*0.000061 - 0.6;
    coord[1] = y*0.000061 - 0.5;
    coord[2] = z;
    return(coord[axis]);
}

void i2c_write(unsigned char addr, unsigned char reg, unsigned char val){
    unsigned char buff[2];

    addr = 0x68 | (addr & 0b111);  // 0 1 0 0 a d r 0  The 0 means write
    buff[0] = reg;
    buff[1] = val;

    i2c_write_blocking(i2c_default, addr, buff, 2, false);
}
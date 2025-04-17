#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PI_RX 16
#define DAC_CS   17
#define PI_SCK  18
#define PI_TX 19

#define RAM_CS 15

union FloatInt{
    float f;
    uint32_t i;
};

void connectusb();

static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);
void writeDac(uint8_t *data, int len);
void inwriteDac(uint8_t *data, int channel, float voltage);

void spi_pi_init();
void spi_ram_init();
float read_ram(uint16_t address);
void write_ram(uint16_t address, float data);

int main()
{
    connectusb();

    //initialization chunk
    spi_pi_init();
    spi_ram_init();
    printf("Pi and Ram initialized.\r\n");

    // Write voltage floats to memory loop
    int t = 0;
    float va;
    uint16_t ad = 0;
    for (int i = 0; i < 1000; i++) {
        va = 1.65*sin(2.*M_PI*(t/1000.)) + 1.65;
        write_ram(ad, va);

        ad += sizeof(float);    // Next address
        t += 1;                 // Next time step
    }

    ad = 0;
    uint8_t vdata[2];
    float voltage;
    t = 0;

    while (1){
        voltage = read_ram(ad);     // Read the float from RAM
        inwriteDac(vdata, 0, voltage);     // Write to DAC  
        writeDac(vdata, 2);
        ad += sizeof(float);
        t += 1;
        sleep_ms(1);

        if (t == 1000){     // Reset at end of data
            ad = 0;
            t = 1;
        }
    }
   
}

void spi_pi_init(){
    spi_init(SPI_PORT, 10000000);

    // These are for the DAC
    gpio_set_function(PI_RX, GPIO_FUNC_SPI);
    gpio_set_function(DAC_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PI_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PI_TX, GPIO_FUNC_SPI);

    // These are for the ram
    gpio_set_function(RAM_CS, GPIO_FUNC_SIO);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(DAC_CS, GPIO_OUT);
    gpio_put(DAC_CS, 1);
    gpio_set_dir(RAM_CS, GPIO_OUT);
    gpio_put(RAM_CS, 1);
}

void spi_ram_init(){
    uint8_t buff[2];
    buff[0] = 0b00000001;    // Change status register
    buff[1] = 0b01000000;    // to sequential mode

    cs_select(RAM_CS);
    spi_write_blocking(SPI_PORT, buff, 2); // where data is a uint8_t array with length len
    cs_deselect(RAM_CS);
}

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void write_ram(uint16_t address, float mynum){
    uint8_t buff[7];
    buff[0] = 0b00000010;       // write instruction
    buff[1] = address >> 8;     // first 8 bits of address
    buff[2] = address & 0xFF;    // last 8 bits of address

    union FloatInt num;
    num.f = mynum;

    buff[3] = (num.i >> 24)&0xFF;
    buff[4] = (num.i >> 16)&0xFF;
    buff[5] = (num.i >> 8)&0xFF;
    buff[6] = num.i & 0xFF;

    cs_select(RAM_CS);
    spi_write_blocking(SPI_PORT, buff, 7);
    cs_deselect(RAM_CS);

}

float read_ram(uint16_t address){
    uint8_t out_buff[7], in_buff[7];
    out_buff[0] = 0b00000011;       // Read instruction
    out_buff[1] = address >> 8;     // First 8 bits of address
    out_buff[2] = address & 0xFF;   // Last 8 bits of address

    out_buff[3] = 0;
    out_buff[4] = 0;
    out_buff[5] = 0;
    out_buff[6] = 0;

    cs_select(RAM_CS);
    spi_write_read_blocking(SPI_PORT, out_buff, in_buff, 7);
    cs_deselect(RAM_CS);

    union FloatInt num;
    num.i = 0;
    num.i = in_buff[3] << 24 | in_buff[4] << 16 | in_buff[5] << 8 | in_buff[6];

    return num.f;
}

void connectusb(){
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Connected!\n");
}

// Write to pin
void writeDac(uint8_t *data, int len){
    cs_select(DAC_CS);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(DAC_CS);
}

// Step 1: Function that takes channel and voltage [0 - 1023] as inputs
void inwriteDac(uint8_t *data, int channel, float voltage){
    int len = 2;
    uint16_t d = 0;

    // Need to put the 16 bit number in the right spot
    d = d | (channel << 15);    // bit shifting to choose output A
    d = d | 0b111 << 12;        // we've been told the next 3 bits are all 1

    // Voltage stuff
    uint16_t v = (voltage * 1023) / 3.3;
    d = d | v << 2;     // Need the last 2 bits of voltage to be zero
    data[0] = d >> 8;   // Grab the 4 channel stuff and first 4 voltage bits
    data[1] = d & 0xFF; // set the final 8 bits;
}

// // Test 1 success
// float testin = 12345.;
// uint16_t a = 0;
// while (1){
//     testin += 1;
//     write_ram(a,testin);
//     float testout;
//     testout = read_ram(a);
//     printf("Test input: %f\r\n", testin);
//     printf("Test output: %f\r\n", testout);
//     printf("\r\n");
//     sleep_ms(1000);
// }

// // Test 2: Load 3 floats, return 3 floats Success
// uint16_t address = 0;
// float test_values[3] = { 123.0f, 456.0f, 789.0f };
// for (int i = 0; i < 3; i++) {
//     write_ram(address, test_values[i]);
//     address += sizeof(float);
// }
// address = 0;
// for (int i = 0; i < 3; i++) {
//     float result = read_ram(address);
//     printf("Read back float %d: %f\r\n", i, result);
//     address += sizeof(float);
// }

// // Test 3: Load 10 floats, return 10 floats Success
// float b = 0;
// uint16_t ad = 0;
// for (int i = 0; i < 10; i++) {
//     write_ram(ad, b);
//     ad += sizeof(float); 
//     b += 1.0f;
// }
// printf("Floats loaded. \r\n");
// ad = 0;
// float b_out;
// for (int i = 0; i < 10; i++){
//     b_out = read_ram(ad);
//     printf("Output: %f\r\n",b_out);
//     ad += sizeof(float);
//     sleep_ms(500);
// }
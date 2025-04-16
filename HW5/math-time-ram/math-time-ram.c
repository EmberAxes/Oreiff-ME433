#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

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

static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);

void spi_pi_init();
void spi_ram_init();
float read_ram(uint16_t address);
void write_ram(uint16_t address, float data);


int main()
{
    //initialization chunk
    stdio_init_all();
    spi_pi_init();
    spi_ram_init();
    
    
}

void spi_pi_init(){
    spi_init(SPI_PORT, 1000*1000);

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
    buff[0] = 0b00000101;    // Change status register
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
    spi_write_read_blocking(spi_default, out_buff, in_buff, 7);
    cs_deselect(RAM_CS);

    union FloatInt num;
    num.i = 0;
    num.i = in_buff[3] << 24 | in_buff[4] << 16 | in_buff[5] << 8 | in_buff[6];

    return num.f
}
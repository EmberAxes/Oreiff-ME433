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

void spi_pi_init();
void spi_ram_init();

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


int main()
{
    stdio_init_all();

    spi_pi_init();
    
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
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
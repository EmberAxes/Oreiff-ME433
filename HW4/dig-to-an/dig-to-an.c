#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   20
#define PIN_SCK  18
#define PIN_MOSI 19

static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);
void writeDac(uint8_t *data, int len);
void inwriteDac(uint8_t *data, int channel, float voltage);

int main(){
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(spi_default, 10000000); // the baud, or bits per second (1000*1000)
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PIN_CS); // PIN_NUM without the GP
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    int t = 0;
    float va;
    float vb;
    uint8_t adata[2];
    uint8_t bdata[2];

    while (true) {
        // Channel A (0) will be the 2Hz sin wave
        // Channel B (1) will be the 1 Hz triangle wave
        va = 1.65*sin(4.*M_PI*(t/1000.)) + 1.65;
        inwriteDac(adata, 0, va);
        writeDac(adata, 2);

        vb = 3.3*(t/1000.);        
        inwriteDac(bdata, 1, vb);       
        writeDac(bdata, 2);
        sleep_ms(1);
        t++;
        if (t == 1000){
            t = 0;
        }
        // Channel B (1) will be the 1Hz triangle wave
    }
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

// Write to pin
void writeDac(uint8_t *data, int len){
    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
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

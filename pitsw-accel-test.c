/**
 * First Pico code for multiple LEDs
 * TEST TO GET ACCELEROMTER WORKING
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"


// By default the accelerometer is on bus address 0x18
const int LIS3DH_ADDRESS = 0x18;
const uint8_t LIS3DH_CTRL_REG_1 = 0x20;
const uint8_t LIS3DH_CTRL_REG_4 = 0x23;

#ifdef i2c_default

void lis3dh_init() {
    uint8_t buf[2];

    // Turn normal mode and 1.344kHz data rate on
    buf[0] = LIS3DH_CTRL_REG_1;
    buf[1] = 0x97;
    i2c_write_blocking(i2c_default, LIS3DH_ADDRESS, buf, 2, false);
}

// Convert with respect to the value being temperature or acceleration reading 
void lis3dh_calc_value(uint16_t raw_value, float *final_value) {
    float scaling;
    float senstivity = 0.004f; // g per unit

    scaling = 64 / senstivity;

    // raw_value is signed
    *final_value = (float)((int16_t)raw_value) / scaling;
}

// Read two bytes of data and store in a 16 bit data structure
void lis3dh_read_data(uint8_t reg, float *final_value) {
    uint8_t lsb;
    uint8_t msb;
    uint16_t raw_accel;

    // FIXME: why does this (and following) use "i2c_default" ??? 
    i2c_write_blocking(i2c_default, LIS3DH_ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c_default, LIS3DH_ADDRESS, &lsb, 1, false);

    reg |= 0x01;
    i2c_write_blocking(i2c_default, LIS3DH_ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c_default, LIS3DH_ADDRESS, &msb, 1, false);

    raw_accel = (msb << 8) | lsb;

    lis3dh_calc_value(raw_accel, final_value);
}

#endif

void init_accel(i2c_inst_t *i2c) {

    // Initialize I2C
    // Use I2C0 on the default SDA and SCL pins (4 and 5 on a Pico)
    i2c_init(i2c, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    lis3dh_init();
}

/** 
 * return whatever is useful to note movemet.
*/
#define THRESHOLD 0.1
bool get_wand_movement() {
    // float x_accel;
    float y_accel;
    // float z_accel;

    // lis3dh_read_data(0x28, &x_accel);
    lis3dh_read_data(0x2A, &y_accel);
    // lis3dh_read_data(0x2C, &z_accel);

    return (abs(y_accel) > THRESHOLD);
}

void show_accel() {
    float x_accel, y_accel, z_accel;

    lis3dh_read_data(0x28, &x_accel);
    lis3dh_read_data(0x2A, &y_accel);
    lis3dh_read_data(0x2C, &z_accel);

    // Display data
    printf("X=%0.3fg Y=%0.3fg Z=%0.3fg\n", x_accel, y_accel, z_accel);
}


// this gives me enough time to switch over to the terminal to see what's going on.
void delay_startup() {

    sleep_ms(4000);
    printf("Counting down to launch...\n");

    // FIXME: PICO_DEFAULT_LED_PIN is also one of the LEDs we want to control. wha?

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    for (int i=5; i>0; i--) {
        printf("%d...\n", i);

        // PICO_DEFAULT_LED_PIN is a misnomer: it's not on any "pin", but it is GPIO 25 on an RP Pico.
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(100);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(900);
    }
    printf("Launching cran_one...\n");
}

int main() {

    stdio_init_all();
    srand(time(0));

    delay_startup();
    
    init_accel(i2c0);

    while (true) {
        show_accel();
        sleep_ms(1000);
        }

}

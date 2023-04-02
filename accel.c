// accelerometer-oriented code

#include <stdio.h>
#include <stdlib.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"


// By default the accelerometer is on bus address 0x18
const int ADDRESS = 0x18;
const uint8_t CTRL_REG_1 = 0x20;
const uint8_t CTRL_REG_4 = 0x23;


void lis3dh_init() {
    // Turn normal mode and 1.344kHz data rate on
    uint8_t buf[2];
    buf[0] = CTRL_REG_1;
    buf[1] = 0x97;
    i2c_write_blocking(i2c_default, ADDRESS, buf, 2, false);
}

void lis3dh_calc_value(uint16_t raw_value, float *final_value, bool isAccel) {
    // Convert with respect to the value being temperature or acceleration reading 
    float scaling;
    float senstivity = 0.004f; // g per unit

    if (isAccel == true) {
        scaling = 64 / senstivity;
    } 
    else {
        scaling = 64;
    }

    // raw_value is signed
    *final_value = (float) ((int16_t) raw_value) / scaling;
}

void lis3dh_read_data(uint8_t reg, float *final_value, bool IsAccel) {
    // Read two bytes of data and store in a 16 bit data structure
    uint8_t lsb;
    uint8_t msb;
    uint16_t raw_accel;
    i2c_write_blocking(i2c_default, ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c_default, ADDRESS, &lsb, 1, false);

    reg |= 0x01;
    i2c_write_blocking(i2c_default, ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c_default, ADDRESS, &msb, 1, false);

    raw_accel = (msb << 8) | lsb;

    lis3dh_calc_value(raw_accel, final_value, IsAccel);
}

// #endif

void init_accel(i2c_inst_t *i2c) {

    printf("init_accel...\n");

    // This example will use I2C0 on the default SDA and SCL pins (4 and 5 on a Pico)
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

    // lis3dh_read_data(0x28, &x_accel, true);
    lis3dh_read_data(0x2A, &y_accel, true);
    // lis3dh_read_data(0x2C, &z_accel, true);

    return abs(y_accel) > THRESHOLD;
}

void show_accel() {
    float x_accel, y_accel, z_accel;

    printf("show_accel...\n");

    lis3dh_read_data(0x28, &x_accel, true);
    lis3dh_read_data(0x2A, &y_accel, true);
    lis3dh_read_data(0x2C, &z_accel, true);

    // Display data 
    printf("\n Acceleration: X: %0.3fg, Y: %0.3fg, Z: %0.3fg\n", x_accel, y_accel, z_accel);
}

/**
 * Accelerometer-oriented code; for the Adafruit LIS3DH.
 * 
 * FIME: Hard-coded to i2c_default, whatever that is.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "accel.h"

// By default the accelerometer is on I2C bus address 0x18
const int ADDRESS = 0x18;
const uint8_t CTRL_REG_1 = 0x20;
// const uint8_t CTRL_REG_4 = 0x23; - used for temperature sensing, which we don't do.


void test_accel() {
    test_accel_3();
    }

void test_accel_3() {
    while (true) {
        printf("\e[1;1H\e[2J");
        show_accel();
        sleep_ms(1000);
        }
    }

void test_accel_1() {
    while (true) {
        show_accel();
        sleep_ms(1000);
        }
    }

void test_accel_2() {
    while (true) {
        if (detect_wand_movement()) {
            show_accel();
            sleep_ms(1000);
            }
        }
    }


/**
 * Initialize the accelerometer hardware.
*/
void lis3dh_init() {

    printf("Into lis3dh_init\n");

    // Turn normal mode and 1.344kHz data rate on
    uint8_t buf[2];
    buf[0] = CTRL_REG_1;
    buf[1] = 0x97;
    i2c_write_blocking(i2c_default, ADDRESS, buf, 2, false);

    printf("out of lis3dh_init\n");
    }

void lis3dh_calc_value(uint16_t raw_value, float *final_value) {
    // Convert with respect to the value being temperature or acceleration reading 
    static const float scaling = 64.0f / 0.004f; // g per unit; magic number from example code

    // raw_value is signed
    *final_value = (float) ((int16_t) raw_value) / scaling;
    }

void lis3dh_read_data(uint8_t reg, float *final_value) {
    uint8_t lsb;
    uint8_t msb;
    uint16_t raw_accel;

    i2c_write_blocking(i2c_default, ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c_default, ADDRESS, &lsb, 1, false);

    reg |= 0x01;
    i2c_write_blocking(i2c_default, ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c_default, ADDRESS, &msb, 1, false);

    raw_accel = (msb << 8) | lsb;

    lis3dh_calc_value(raw_accel, final_value);
    }


/**
 * Initialize I2C and the accelerometer hardware.
*/
void init_accel() {

    printf("init_accel...\n");

    // FIXME: this is hardcoded to i2c_default

    // This example will use i2c_default at 400 Kbaud, 
    // on the default SDA and SCL pins (4 and 5 on a Pico, 2 and 3 on a Feather).
    //
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    printf("I2C init OK....\n");

    // Make the I2C pins available to picotool - FIXME: not used?
    // bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    lis3dh_init();
    printf("init_accel OK!\n");
    }

/** 
 * return whatever is useful to note movement.
*/
#define THRESHOLD 1.5
bool detect_wand_movement() {
    // float x_accel;
    float y_accel;
    // float z_accel;

    // lis3dh_read_data(0x28, &x_accel);
    lis3dh_read_data(0x2A, &y_accel);
    // lis3dh_read_data(0x2C, &z_accel);


    return y_accel > THRESHOLD;
    // return abs(y_accel) > THRESHOLD;

    }

void show_accel() {
    float x_accel, y_accel, z_accel;

    lis3dh_read_data(0x28, &x_accel);
    lis3dh_read_data(0x2A, &y_accel);
    lis3dh_read_data(0x2C, &z_accel);

    // Display data 
    printf("Acceleration: X: %0.3fg, Y: %0.3fg, Z: %0.3fg\n", x_accel, y_accel, z_accel);
    }

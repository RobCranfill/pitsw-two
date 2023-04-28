/**
 * Accelerometer-oriented code; for the Adafruit LIS3DH.
 * 
 * FIXME: Hard-coded to i2c_default, whatever that is.
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
const uint8_t CTRL_REG_1 = 0x20; // FIXME: why not a #define?
// const uint8_t CTRL_REG_4 = 0x23; - used for temperature sensing, which we don't do.

#define X_ACCEL_REG    0x28
#define Y_ACCEL_REG    0x2A
#define Z_ACCEL_REG    0x2C


// we run a test - which one?
// does not return, probably
void test_accel() {

    // test_accel_1();
    // test_accel_2();
    // test_accel_3();
    // test_accel_4();
    test_accel_5();
    }


#define Y_THRESHOLD 1.5f
// 10, 10 works perfectly; so does 1,1 FIXME: NEEDED?
#define DEBOUNCE_SLEEP_MS   1
#define ACCEL_READ_SLEEP_MS 1

/** 
 * return whatever is useful to note movement.
 * This will wait for the start of a swipe, and return true then.
*/
bool detect_wand_movement() {

    float y_accel;
    int i = 0;

    while (true) {
        lis3dh_read_data(Y_ACCEL_REG, &y_accel);
        if (y_accel < Y_THRESHOLD) {
            sleep_ms(ACCEL_READ_SLEEP_MS);
            }
        else
            {
            // just for fun show timings; remove later
            absolute_time_t at = get_absolute_time();
            printf("%d\t\%d\t", i++, at._private_us_since_boot);
            printf("   y_accel %4.1f\n -> ", y_accel);

            // debounce
            bool hasGoneLow = false;
            while (!hasGoneLow) {
                lis3dh_read_data(Y_ACCEL_REG, &y_accel);
                if (y_accel < Y_THRESHOLD) {
                    hasGoneLow = true;
                    printf("** debounce!\n");
                    sleep_ms(DEBOUNCE_SLEEP_MS);
                    }
                }

            }
        }

    }


// how to detect one swipe of the wand?
// how to de-bounce the input?
void test_accel_5() {

    float y_accel;
    int i = 0;

    while (true) {
        sleep_ms(ACCEL_READ_SLEEP_MS);
        lis3dh_read_data(Y_ACCEL_REG, &y_accel);
        if (y_accel > Y_THRESHOLD) {
            absolute_time_t at = get_absolute_time();
            printf("%d\t\%d\t", i++, at._private_us_since_boot);
            printf("   y_accel %4.1f\n -> ", y_accel);

            bool hasGoneLow = false;
            while (!hasGoneLow) {
                lis3dh_read_data(Y_ACCEL_REG, &y_accel);
                if (y_accel < Y_THRESHOLD) {
                    hasGoneLow = true;
                    printf("** debounce!\n");
                    sleep_ms(DEBOUNCE_SLEEP_MS);
                    }
                }

            }
        }

    }


// output text values for analysis - de-bounce?
void test_accel_4() {

    int pause_ms = 100;
    printf("Test mode 4\n");
    printf("pause_ms: %d\n", pause_ms);
    printf("t_boot\tx\ty\tz\n");

    int i = 0;
    while (true) {
        absolute_time_t foo = get_absolute_time();
        printf("%d\t\%d\t", i++, foo._private_us_since_boot);
        show_accel_tabbed();
        sleep_ms(pause_ms);
        }
    }

void test_accel_3() {
    while (true) {
        printf("\e[1;1H\e[2J");
        show_accel();
        sleep_ms(1000);
        }
    }

void test_accel_2() {
    int i = 0;
    while (true) {
        if (detect_wand_movement()) {
            printf("trigger #%d\n", i++);
            // show_accel();
            sleep_ms(500);
            }
        }
    }

void test_accel_1() {
    while (true) {
        show_accel();
        sleep_ms(1000);
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

    printf("init_accel using i2c_default...\n");

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


float lis3dh_read_y_accel(void) {
    float y_accel;
    lis3dh_read_data(X_ACCEL_REG, &y_accel);
    return y_accel;
    }

void show_accel() {
    float x_accel, y_accel, z_accel;

    lis3dh_read_data(X_ACCEL_REG, &x_accel);
    lis3dh_read_data(Y_ACCEL_REG, &y_accel);
    lis3dh_read_data(Z_ACCEL_REG, &z_accel);

    // Display data 
    printf("%0.3f, %0.3f, %0.3f\n", x_accel, y_accel, z_accel);
    }

void show_accel_tabbed() {
    float x_accel, y_accel, z_accel;

    lis3dh_read_data(X_ACCEL_REG, &x_accel);
    lis3dh_read_data(Y_ACCEL_REG, &y_accel);
    lis3dh_read_data(Z_ACCEL_REG, &z_accel);

    // Display data 
    printf("%0.3f\t%0.3f\t%0.3f\n", x_accel, y_accel, z_accel);
    }

/**
 * First Pico code for multiple LEDs
 * Draws an "X". 
 * Uses an accelerometer trigger the display scan.
 * 
 * TODO: for speed, use gpio_put_masked() ??? (or similar)
 *  fancy hot chocoloate
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

// By default the accelerometer is on bus address 0x18
const int ADDRESS = 0x18;
const uint8_t CTRL_REG_1 = 0x20;
const uint8_t CTRL_REG_4 = 0x23;

#define BUTTON_GPIO 15

// mapping from LED index to GPIO number
#define N_LEDS  8
int led_mapping[] = {16, 17, 18, 19, 26, 22, 21, 20};

// This array is N_COLS high, so to speak, and N_LEDS wide.
#define N_COLS  10
int led_data[N_COLS][N_LEDS] = {
    {1,0,0,0,0,0,0,1},
    {0,1,0,0,0,0,1,0},
    {0,0,1,0,0,1,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,1,0,0,1,0,0},
    {0,1,0,0,0,0,1,0},
    {1,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
};

// flag for verifying thread startup
#define WORKER_STARTED_FLAG 666

#ifdef i2c_default

void lis3dh_init() {
    uint8_t buf[2];

int integer1, integer2, sum;
printf("Sum of %d ", integer1," and %d", integer2," is: %d\n", sum);

    // Turn normal mode and 1.344kHz data rate on
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
    } else {
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

#endif

void init_accel(i2c_inst_t *i2c) {

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

    return (abs(y_accel) > THRESHOLD);
}

void show_accel() {
    float x_accel, y_accel, z_accel;

    lis3dh_read_data(0x28, &x_accel, true);
    lis3dh_read_data(0x2A, &y_accel, true);
    lis3dh_read_data(0x2C, &z_accel, true);

    // Display data 
    printf("\n Acceleration: X: %0.3fg, Y: %0.3fg, Z: %0.3fg\n", x_accel, y_accel, z_accel);
}

/* Turn the LED on for just a tad (and sleep after, for spacing?)
*/
void flash(int gpio, int on_time) {
    gpio_put(gpio, 1);
    sleep_ms(on_time);
    gpio_put(gpio, 0);
}

/*
    Quickly blink X number of times, then wait a second.
*/
void run_leds() {

    printf("run_leds launched!\n");

    // let master know we have started
    multicore_fifo_push_blocking(WORKER_STARTED_FLAG);

    int delay = (int)multicore_fifo_pop_blocking();
    printf("    run_leds delay = %d\n", delay);

    while (true) {

        while (!get_wand_movement()) {
            tight_loop_contents(); // needed????
            }

        printf("FIRING!\n");

        // if (multicore_fifo_rvalid()) {
        //     delay = (int)multicore_fifo_pop_blocking();
        //     printf("    run_leds got updated delay = %d\n", delay);
        //     }

        for (int col=0; col<N_COLS; col++) {
            for (int i=0; i<N_LEDS; i++) {
                // printf("setting led %d to %d\n", i, led_data[i][col]);
                gpio_put(led_mapping[i], led_data[col][i]);
            }
            sleep_ms(delay);

        }

    }
    printf("    **** worker done (shouldn't happen?)\n");
}

void init_leds() {
    for (int i=0; i<N_LEDS; i++) {
        gpio_init(led_mapping[i]);
        gpio_set_dir(led_mapping[i], GPIO_OUT);
    }
}

// this gives me enough time to switch over to the terminal to see what's going on.
void delay_startup() {
    sleep_ms(4000);
    printf("Counting down to launch...\n");
    for (int i=3; i>0; i--) {
        printf("%d...\n", i);

        // PICO_DEFAULT_LED_PIN is a misnomer: it's not on any "pin", but it is GPIO 25
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
    
printf("init_leds...\n");
    init_leds();

printf("cran_init_accel...\n");
    init_accel(i2c1);   // we are using i2c1, the alternate one, cuz it works better on the breadboard :-)

printf("cran_show_accel...\n");
    show_accel();

    // set up increment button
    gpio_init(BUTTON_GPIO);
    gpio_set_dir(BUTTON_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON_GPIO);


    // Launch worker and wait for it to start up
    multicore_launch_core1(run_leds);
    uint32_t g = multicore_fifo_pop_blocking();
    if (g != WORKER_STARTED_FLAG) {
        printf("Error: Worker didn't start? Stopping.\n");
        exit(1);
    }
    printf("Worker started. Sending messages....\n");

    multicore_fifo_push_blocking(1);    // does this block *this* thread?

    while (true) {

        if (multicore_fifo_wready()) {

            // uint32_t r = (rand() % 10) + 1;
            // printf("\nMaster requesting delay of %d ...\n", r);
            // multicore_fifo_push_blocking(r);    // does this block *this* thread?

            printf("Master sleeping for 10 seconds...\n");
            sleep_ms(10000);      // IS THIS OK? DOES THIS BLOCK THE OTHER CORE????
        }
        else {
            printf("Worker not ready????");
            sleep_ms(5000);
        }

    }
}
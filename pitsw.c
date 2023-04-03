/**
 * Pi-In-The-Sky-Writer 2, for RP2040
 * (c)2023 robcranfill@gmail.com
 * 
 * Draws an "X"
 * Uses an accelerometer trigger the display scan.
 * 
 * TODO: for speed, use gpio_put_masked() (or similar) ??
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"

#include "accel.h"
#include "font.h"

// Is this the right way to do this?? rusty C ! :)
#define PICO_PICO 0
#define PICO_FEATHER 1

#define N_LEDS  8
#if PICO_PICO
#define BUTTON_GPIO 15
// mapping from LED index to GPIO number
int led_mapping[] = {16, 17, 18, 19, 26, 22, 21, 20};
#endif

#if PICO_FEATHER
#define BUTTON_GPIO 25
// mapping from LED index to GPIO number
int led_mapping[] = {6, 7, 8, 9, 10, 11, 12, 13};
#endif

// FIXME: why only this one???
uint32_t setBits(int n_bits, int *list_of_bit_numbers);

// // This array is N_COLS wide, so to speak, and N_LEDS high.
// #define N_COLS  10
// int led_data_X[N_COLS][N_LEDS] = {
//     {1,0,0,0,0,0,0,1},
//     {0,1,0,0,0,0,1,0},
//     {0,0,1,0,0,1,0,0},
//     {0,0,0,1,1,0,0,0},
//     {0,0,0,1,1,0,0,0},
//     {0,0,1,0,0,1,0,0},
//     {0,1,0,0,0,0,1,0},
//     {1,0,0,0,0,0,0,1},
//     {0,0,0,0,0,0,0,0},
//     {0,0,0,0,0,0,0,0}
// };

// flag for verifying thread startup
#define WORKER_STARTED_FLAG 666


// /* Turn the given LED on for just a tad.
//     I don't think this is useful.
// */
// void flash(int gpio, int on_time_ms) {
//     gpio_put(gpio, 1);
//     sleep_ms(on_time_ms);
//     gpio_put(gpio, 0);
// }

/*
    Quickly blink X number of times, then wait a second.
*/
void run_leds() {

    printf("run_leds launched!\n");

    // let master know we have started
    multicore_fifo_push_blocking(WORKER_STARTED_FLAG);

    int led_hold_time_ms = (int)multicore_fifo_pop_blocking();
    printf("    run_leds led_hold_time_ms = %d\n", led_hold_time_ms);

    while (true) {

        while (!get_wand_movement()) {
            tight_loop_contents(); // needed??
            }

        printf("firing LEDs with pause of %d ms\n", led_hold_time_ms);

        if (multicore_fifo_rvalid()) {
            led_hold_time_ms = (int)multicore_fifo_pop_blocking();
            printf("    run_leds got updated led_hold_time_ms = %d\n", led_hold_time_ms);
            }

        // where the rubber meets the road.
        //
        uint32_t led_mask = setBits(8, led_mapping);
        uint32_t *led_data = getVRasterForChar('X'); // TODO:

        printf("led_mask = %012o \n", led_mask);

        for (int i=0; i<8; i++) {
            gpio_put_masked(led_mask, led_data[i] << 6);
            printf("led_data[i] = %012o \n", led_data[i] << 6);
            sleep_ms(led_hold_time_ms);
            }

        // for (int col=0; col<N_COLS; col++) {
        //     for (int i=0; i<N_LEDS; i++) {
        //         // printf("setting led %d to %d\n", i, led_data_to_use[col][i]);
        //         gpio_put(led_mapping[i], led_data_to_use[col][i]);
        //         }
        //     sleep_ms(led_hold_time_ms);
        //     }

        }
    printf("    **** worker done (shouldn't happen?)\n");
    }

/**
 * Set GPIO pins for LEDs to output, no pullup. TODO: right?
*/
void init_leds() {
    printf("init_leds...\n");
    printf("Setting pins to output:");
    for (int i=0; i<N_LEDS; i++) {
        uint p = led_mapping[i];
        printf(" %d", p);
        gpio_init(p);
        gpio_set_dir(p, GPIO_OUT);
        }
    printf("\n");
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
    printf("Launching...\n");
    }

uint32_t setBits(int n_bits, int *list_of_bit_numbers) {
    uint32_t bits = 0;
    for (int i=0; i<n_bits; i++) {
        bits |= (1 << list_of_bit_numbers[i]);
        // printf(" bits are now %04o\n", bits);
        }
    return bits;
    }


void gpio_test() {
    uint32_t the_mask = 0, the_data = 0;

    init_leds();

    the_mask = setBits(8, led_mapping);

    int raw_data[] = {6, 8, 10, 12};
    the_data = setBits(4, raw_data);

    printf("the_mask = %012o, the_data = %012o\n", the_mask, the_data);

    for (int i=1; i<30; i++) {
        gpio_put_masked(the_mask, the_data);
        sleep_ms(100);
        gpio_put_masked(the_mask, 0);
        sleep_ms(100);
        }
    printf("DONE!\n");
    }

int main() {

    stdio_init_all();
    srand(time(0));

    delay_startup();    // so I have time to crank up the term program and see things from the start

    // GPIO test
    // gpio_test();
    // printf("EXITING EARLY!");
    // exit(1);

    // font_test_3();
    // printf("EXITING EARLY!");
    // exit(1);

    init_leds();
    init_accel(i2c1);   // we are using i2c1, the alternate one, cuz it works better on the breadboard :-)

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

/*
    lessons learned:
        main_sleep_sec = 1 -> compressed text
        main_sleep_sec = 3-5 -> OK?
        main_sleep_sec = 10 -> way too wide text
*/
    int main_sleep_sec = 10; // this is just for testing... right?

    while (true) {

        if (multicore_fifo_wready()) {

            uint32_t r = (rand() % 10) + 1;
            printf("\nMaster requesting main_sleep_sec of %d ...\n", r);
            multicore_fifo_push_blocking(r);    // does this block *this* thread?

            printf("Master sleeping for %d seconds...\n", main_sleep_sec);
            sleep_ms(main_sleep_sec * 1000);      // This does not block the other core!
            }
        else {
            printf("Worker not ready????");
            sleep_ms(5000);
            }
        } // while true
    }
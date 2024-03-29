/**
 * Pi-In-The-Sky-Writer 2, for RP2040
 * (c)2023 robcranfill@gmail.com
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

// FIXME: a forward declaration - why only this one???
uint32_t makeBitmask(int n_bits, int *list_of_bit_numbers);

// Select the hardware to compile for.
// Is this the right way to do this?? rusty C ! :)
#define PICO_PICO 0
// FIXME: can i do something like #define PICO_FEATHER (NOT PICO_PICO) ??
#define PICO_FEATHER 1

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

// How tall our display is.
#define N_LEDS  8

// flag for verifying thread startup
#define WORKER_STARTED_FLAG 666


#define Y_THRESHOLD 1.5f
// 10, 10 works perfectly; so does 1,1 FIXME: are these even needed?
#define DEBOUNCE_SLEEP_MS   1
#define ACCEL_READ_SLEEP_MS 1


/// @brief This is the never-ending task the 2nd core runs.
void run_leds() {

    printf("run_leds launched!\n");

    // let master know we have started
    multicore_fifo_push_blocking(WORKER_STARTED_FLAG);

    int led_hold_time_ms = (int)multicore_fifo_pop_blocking();
    printf("    run_leds led_hold_time_ms = %d\n", led_hold_time_ms);

    uint32_t led_mask = makeBitmask(N_LEDS, led_mapping);
    printf("led_mask = %012o \n", led_mask);

    char *msg = "TX-O";
    int swipe_count = 0;

    uint32_t *led_data = getRastersForStr(msg);
    int bits_wide_to_display = 16; // FIXME: variable? depends on ???
    int led_data_start = 0;

    int t_start = get_absolute_time()._private_us_since_boot;

    // This loops forever, watching for a wand movement. When it gets one, it runs the LEDs once.
    //
    while (true) {

        // Display the current chunk of message.
        //

        float y_accel = lis3dh_read_y_accel();
        if (y_accel < Y_THRESHOLD) {
            tight_loop_contents();  // FIXME: or sleep?
            }
        else
            {

            // just for fun show timings; remove later
            swipe_count++;
            float t_delta = (get_absolute_time()._private_us_since_boot - t_start) / 1000000;

            printf("s=%d t=%4.1f d=%d @%d \n", swipe_count, t_delta, led_hold_time_ms, led_data_start);
            // printf(" >>>>>>>> s=%d d=%d \n", swipe_count, led_hold_time_ms);
            // printf("   y_accel %4.1f\n \n", y_accel);

            for (int i=bits_wide_to_display; i>=0; i--) {
                // bitmap akligns when shifted over 6 places
                gpio_put_masked(led_mask, led_data[led_data_start + i] << 6);

                printf("led_data[%02d] = %08b \n", led_data_start + i, led_data[led_data_start + i]);

                sleep_ms(led_hold_time_ms);
                }


            // debounce - wait for the acc to go low again.
            bool hasGoneLow = false;
            while (!hasGoneLow) {

                y_accel = lis3dh_read_y_accel();
                if (y_accel < Y_THRESHOLD) {
                    hasGoneLow = true;
                    printf("** debounce!\n");
                    // sleep_ms(DEBOUNCE_SLEEP_MS);
                    }
                else {
                    tight_loop_contents();  // FIXME: or sleep?
                    }
                }

            }

#if 0
        while (!detect_wand_movement()) {
            // TODO: i don't understand this. this blocks or returns or what?
            // @see https://forums.raspberrypi.com/viewtopic.php?t=349804
            tight_loop_contents();
            }
#endif

        // printf("firing LEDs with pause of %d ms\n", led_hold_time_ms);

        // Check for a message from the 'main' thread/core, the one that launched this one.
        //
        // FIXME: for now this is the delay time, but this perhaps should be the message to display?
        //
        if (multicore_fifo_rvalid()) {
            led_hold_time_ms = (int)multicore_fifo_pop_blocking();
            printf("    run_leds got updated led_hold_time_ms = %d\n", led_hold_time_ms);
            }

#if 0
        // FIXME: for test, increment here
        swipe_count += 1;
        if (swipe_count > 10) {
            swipe_count = 0;
            msg_index += 1;
            if (msg_index >= sizeof(msg)) {
                msg_index = 0;
                }
            led_data = getVRasterForChar(msg[msg_index]);
            printf(" new character: '%c' \n", msg[msg_index]);
            }
#endif

        }
    printf("    **** worker done (shouldn't happen?)\n"); // TODO: is there some shutdown thing I can do?
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

/// @brief Give me enough time to crank up the term program and see all the output.
void delay_startup() {
    sleep_ms(4000);
    printf("Counting down to launch...\n");
    for (int i=5; i>0; i--) {
        printf("%d...\n", i);

        // PICO_DEFAULT_LED_PIN is a misnomer: it's not on any "pin", but it is GPIO 25
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(500);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(500);
        }
    printf("Launching...\n");
    }


/// @brief Create a bitmask, setting just those bits listed.
/// @param n_bits TODO: can we git rid of this and just pass in a list? No, this is C!
/// @param list_of_bit_numbers
/// @return the bitmask
uint32_t makeBitmask(int n_bits, int *list_of_bit_numbers) {
    uint32_t bits = 0;
    for (int i=0; i<n_bits; i++) {
        bits |= (1 << list_of_bit_numbers[i]);
        // printf(" bits are now %04o\n", bits);
        }
    return bits;
    }


/// @brief for testing
void gpio_test() {
    uint32_t the_mask = 0, the_data = 0;

    init_leds();

    the_mask = makeBitmask(N_LEDS, led_mapping);

    int raw_data[] = {6, 8, 10, 12};
    the_data = makeBitmask(4, raw_data);

    printf("the_mask = %012o, the_data = %012o\n", the_mask, the_data);

    for (int i=1; i<30; i++) {
        gpio_put_masked(the_mask, the_data);
        sleep_ms(100);
        gpio_put_masked(the_mask, 0);
        sleep_ms(100);
        }
    printf("gpio_test done!\n");
    }

// private void test_led_13(void) {
//     gpio_put_masked(the_mask, the_data);
//     sleep_ms(100);
//     }

int main() {

    stdio_init_all();
    srand(time(0));
    init_leds(); // do this early so LEDs can flash at startup?
    delay_startup();    // so I have time to crank up the term program and see things from the start

#if 0
    test_led_13(); // does not return
#endif

#if 0
    init_accel();
    test_accel(); // never returns
#endif

#if 0
    GPIO test
    gpio_test();
    printf("done with test!");
    exit(1);
#endif

#if 0
    font_test();
    printf("done with font test!");
    exit(1);
#endif

    // Initialize I2C and the accelerometer hardware.
    //
    // init_accel(i2c1);   // we are using i2c1, the alternate one, cuz it works better on the breadboard :-)
    init_accel();   // FIXME: NO WE ARE NOT! 

#if 0
    // set up increment button - so far unused TODO:
    gpio_init(BUTTON_GPIO);
    gpio_set_dir(BUTTON_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON_GPIO);
#endif

    // Launch worker and wait for it to start up
    multicore_launch_core1(run_leds);
    uint32_t flag = multicore_fifo_pop_blocking(); // gets "started" response
    if (flag != WORKER_STARTED_FLAG) {
        printf("Error: Worker didn't start? Got %d. Stopping.\n", flag);
        exit(1);
        }
    printf("Worker started. Sending messages....\n");

    // FIXME: not needed? yes, it's needed, it's the initial value of led_hold_time_ms, right?
    multicore_fifo_push_blocking(5);    // does this block *this* thread?

/*
    lessons learned:
        main_sleep_sec = 1 -> compressed text
        main_sleep_sec = 3-5 -> OK?
        main_sleep_sec = 10 -> way too wide text
*/
    int main_sleep_sec = 10; // TODO: this is for main thread event loop - smaller later to handle GUI?

    while (true) {

        if (multicore_fifo_wready()) {

            uint32_t led_delay_ms = (rand() % 10) + 1;
            printf("\nMaster requesting main_sleep_sec of %d ...\n", led_delay_ms);
            multicore_fifo_push_blocking(led_delay_ms);    // does this block *this* thread? no!

            printf("Master thread sleeping for %d seconds...\n", main_sleep_sec);
            sleep_ms(main_sleep_sec * 1000);      // This does not block the other core!
            }
        else {
            printf("Worker not ready? Sleeping 5 seconds....\n");
            sleep_ms(5000);
            }
        } // while true
    }

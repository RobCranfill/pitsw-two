/*
 * First Pico code for multiple LEDs
 * Draws an "X". 
 * Use a button to increment LED pause.
 * 
 * TODO: for speed, use gpio_put_masked() ??? (or similar)
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#define WORKER_STARTED_FLAG 666

#define BUTTON_GPIO 15

// mapping from LED index to GPIO number
#define N_LEDS  8
int led_mapping[] = {16, 17, 18, 19, 26, 22, 21, 20};

#define N_COLS  10

// This array is N_COLS high, so to speak, and N_LEDS wide.
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

        tight_loop_contents(); // needed????

        // if (multicore_fifo_rvalid()) {
        //     delay = (int)multicore_fifo_pop_blocking();
        //     printf("    run_leds got updated delay = %d\n", delay);
        //     }

        bool button = !gpio_get(BUTTON_GPIO);
        if (button) {
            ++delay;
            if (delay >= 20) {
                delay = 0;
            }
            printf("    run_leds updated delay = %d\n", delay);
            sleep_ms(200); // debounce
        }
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
    sleep_ms(2000);
    printf("Counting down to launch...\n");
    for (int i=5; i>0; i--) {
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
    
    init_leds();

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
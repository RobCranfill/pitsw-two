/**
 * First Pico code for multiple LEDs
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#define WORKER_STARTED_FLAG 666

// mapping from LED index to GPIO pin number
int N_LEDS = 8;
int led_mapping[] = {16, 17, 18, 19, 26, 22, 21, 20};


/* Turn the LED on for just a tad (and sleep after, for spacing?)
*/
void flash(int pin, int on_time) {
    gpio_put(pin, 1);
    sleep_ms(on_time);
    gpio_put(pin, 0);
}

/*
    Quickly blink X number of times, then wait a second.
*/
void core1_handler() {

    printf("core1_handler launched!\n");

    // let master know we have started
    multicore_fifo_push_blocking(WORKER_STARTED_FLAG);

    int ontime = (int)multicore_fifo_pop_blocking();
    printf("    worker ontime = %d\n", ontime);

    while (true) {

        tight_loop_contents(); // needed????

        if (multicore_fifo_rvalid()) {
            ontime = (int)multicore_fifo_pop_blocking();
            printf("    worker got updated ontime = %d\n", ontime);
            }

        for (int i=0; i<N_LEDS; i++) {
            flash(led_mapping[i], ontime);
        }
    }
    printf("    **** worker done (shouldn't happen?)\n");
}

void init_leds() {
    for (int i=0; i<N_LEDS; i++) {
        printf("Initing pin %d\n", led_mapping[i]);
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


    // Launch worker and wait for it to start up
    multicore_launch_core1(core1_handler);
    uint32_t g = multicore_fifo_pop_blocking();
    if (g != WORKER_STARTED_FLAG) {
        printf("Error: Worker didn't start? Stopping.\n");
        exit(1);
    }
    printf("Worker started. Sending messages....\n");

    multicore_fifo_push_blocking(10);    // does this block *this* thread?

    while (true) {

        if (multicore_fifo_wready()) {

            uint32_t r = (rand() % 50) + 1;
            printf("\nMaster requesting delay of %d ...\n", r);
            multicore_fifo_push_blocking(r);    // does this block *this* thread?

            printf("Master sleeping for 10 seconds...\n");
            sleep_ms(10000);      // IS THIS OK? DOES THIS BLOCK THE OTHER CORE????
        }
        else {
            printf("Worker not ready????");
            sleep_ms(5000);
        }

    }
}
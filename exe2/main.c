#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

struct repeating_timer timer_r;
struct repeating_timer timer_g;

bool timer_callback_r(struct repeating_timer *t) {
    static bool led_state_r = false;
    led_state_r = !led_state_r;
    gpio_put(LED_PIN_R, led_state_r);
    return true;
}

bool timer_callback_g(struct repeating_timer *t) {
    static bool led_state_g = false;
    led_state_g = !led_state_g;
    gpio_put(LED_PIN_G, led_state_g);
    return true;
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, false);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, false);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    static bool timer_running_r = false;
    static bool timer_running_g = false;

    while (true) {
        if (flag_r) {
            flag_r = 0;
            if (!timer_running_r) {
                add_repeating_timer_ms(500, timer_callback_r, NULL, &timer_r);
                timer_running_r = true;
            } else {
                cancel_repeating_timer(&timer_r);
                timer_running_r = false;
                gpio_put(LED_PIN_R, false);
            }
        }

        if (flag_g) {
            flag_g = 0;
            if (!timer_running_g) {
                add_repeating_timer_ms(250, timer_callback_g, NULL, &timer_g);
                timer_running_g = true;
            } else {
                cancel_repeating_timer(&timer_g);
                timer_running_g = false;
                gpio_put(LED_PIN_G, false);
            }
        }
    }
}

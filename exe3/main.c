#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
absolute_time_t start_press;
int led_state = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { 
        start_press = get_absolute_time();
    } else if (events == 0x8) { 
        flag_f_r = 1;
    }
}

int main() {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0); 

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {
        if (flag_f_r) {
            flag_f_r = 0;
            int64_t duracao = absolute_time_diff_us(start_press, get_absolute_time()) / 1000;
            if (duracao > 500) {
                led_state = !led_state;
                gpio_put(LED_PIN_R, led_state);
            }
        }
    }
}

#include "driver/gpio.h"
#include "config.h"
#include "led.h"

void led_init(void) {
    gpio_config_t io_out = {
        .intr_type = GPIO_INTR_DISABLE, 
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL<<LED_PIN),
        .pull_down_en = 0, .pull_up_en = 0
    };
    gpio_config(&io_out);
    gpio_set_level(LED_PIN, 0); // Mặc định tắt LED
}

void led_set(bool state) { 
    gpio_set_level(LED_PIN, state ? 1 : 0); 
}
#include "relay.h"
#include "driver/gpio.h"
#include "config.h"


void relay_init(void) {
    gpio_config_t io_out = {
        .intr_type = GPIO_INTR_DISABLE, 
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL<<RELAY_PIN),
        .pull_down_en = 0, .pull_up_en = 0
    };
    gpio_config(&io_out);
    gpio_set_level(RELAY_PIN, 0);
}

void relay_set(bool state) { 
    gpio_set_level(RELAY_PIN, state ? 1 : 0); 
}

#include "driver/gpio.h"
#include "config.h"
#include "buzzer.h"

void buzzer_init(void) {
    gpio_config_t io_out = {
        .intr_type = GPIO_INTR_DISABLE, 
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL<<BUZZER_PIN),
        .pull_down_en = 0, .pull_up_en = 0
    };
    gpio_config(&io_out);
    gpio_set_level(BUZZER_PIN, 1); // Kích mức THẤP -> Mức 1 là TẮT còi
}

void buzzer_set(bool state) { 
    // Truyền true (bật) -> Xuất mức 0. Truyền false (tắt) -> Xuất mức 1.
    gpio_set_level(BUZZER_PIN, state ? 0 : 1); 
}
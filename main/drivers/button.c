#include "driver/gpio.h"
#include "config.h"
#include "button.h"

void button_init(void) {
    gpio_config_t io_in = {
        .intr_type = GPIO_INTR_DISABLE, 
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL<<BUTTON_PIN),
        .pull_down_en = 0, 
        .pull_up_en = 1 // Bật trở kéo lên nội bộ
    };
    gpio_config(&io_in);
}   

bool button_is_pressed(void) { 
    return gpio_get_level(BUTTON_PIN) == 0; // Nút nhấn kéo xuống GND
}
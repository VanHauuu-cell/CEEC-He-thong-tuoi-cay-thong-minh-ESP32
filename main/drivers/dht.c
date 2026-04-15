#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"
#include "dht.h"

bool dht11_read(float *temp, float *hum) {
    uint8_t data[5] = {0}; 
    gpio_set_direction(DHT_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(20)); 
    gpio_set_level(DHT_PIN, 1);
    ets_delay_us(30);
    gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);

    uint16_t timeout = 0;
    while (gpio_get_level(DHT_PIN) == 1) { if (++timeout > 100) return false; ets_delay_us(1); }
    timeout = 0;
    while (gpio_get_level(DHT_PIN) == 0) { if (++timeout > 100) return false; ets_delay_us(1); }
    timeout = 0;
    while (gpio_get_level(DHT_PIN) == 1) { if (++timeout > 100) return false; ets_delay_us(1); }

    for (int i = 0; i < 40; i++) {
        timeout = 0;
        while (gpio_get_level(DHT_PIN) == 0) { if (++timeout > 100) return false; ets_delay_us(1); }
        uint32_t start_time = esp_timer_get_time(); 
        timeout = 0;
        while (gpio_get_level(DHT_PIN) == 1) { if (++timeout > 100) return false; ets_delay_us(1); }
        if ((esp_timer_get_time() - start_time) > 40) {
            data[i / 8] |= (1 << (7 - (i % 8))); 
        }
    }

    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        *hum = data[0]; 
        *temp = data[2]; 
        return true;  
    }
    return false;     
}

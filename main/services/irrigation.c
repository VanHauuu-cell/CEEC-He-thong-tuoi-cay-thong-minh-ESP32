#include "irrigation.h"
#include "relay.h"
#include "config.h"
#include "fsm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "IRRIGATION";
#define SOIL_THRESHOLD 30
#define RELAY_ON 1
#define RELAY_OFF 0

bool pump_running = false;
static TimerHandle_t pump_timer = NULL;

static void pump_timer_cb(TimerHandle_t timer){
    (void)timer;
    ESP_LOGW(TAG, "Pump timer expired");
    system_event ev = { .event_type = E_PUMP_DONE };
    xQueueSend(system_queue, &ev, 0);
}

void irrigation_init(void){
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << RELAY_PIN),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(RELAY_PIN, RELAY_OFF);

        pump_timer = xTimerCreate(
        "pump_tmr",
        pdMS_TO_TICKS(20000),
        pdFALSE,          
        NULL,
        pump_timer_cb
        );
    ESP_LOGI(TAG, "Irrigation service initialized.");
}

void irrigation_start(void){
    if(pump_running){
        xTimerReset(pump_timer, pdMS_TO_TICKS(100));
        ESP_LOGW(TAG, "PUMP RUNNING");
        return;
    }
    gpio_set_level(RELAY_PIN, RELAY_ON);
    pump_running = true;
    xTimerStart(pump_timer,pdMS_TO_TICKS(100));
    ESP_LOGW(TAG, "PUMP ON");
}

void irrigation_stop(void){
    if(!pump_running){
        return;
    }
    gpio_set_level(RELAY_PIN, RELAY_OFF);
    pump_running = false;
    if(xTimerIsTimerActive(pump_timer) == pdTRUE){
        xTimerStop(pump_timer,pdMS_TO_TICKS(100));
    }
    ESP_LOGW(TAG, "PUMP OFF");
}

bool irrigation_is_running(void){
    return pump_running;
}

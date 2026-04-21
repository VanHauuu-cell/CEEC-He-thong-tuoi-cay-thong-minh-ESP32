#include "irrigation.h"
#include "relay.h"
#include "led.h"
#include "config.h"
#include "fsm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "IRRIGATION";
#define SOIL_THRESHOLD 30


bool pump_running = false;
static TimerHandle_t pump_timer = NULL;

static void pump_timer_cb(TimerHandle_t timer){
    (void)timer;
    ESP_LOGI(TAG, "Pump timer expired");
    system_event ev = { .event_type = E_PUMP_DONE };
    xQueueSend(system_queue, &ev, 0);
}

void irrigation_init(void){
        pump_timer = xTimerCreate(
        "pump_tmr",
        pdMS_TO_TICKS(20000),
        pdFALSE,          
        NULL,
        pump_timer_cb);
     configASSERT(pump_timer != NULL);
    ESP_LOGI(TAG, "Irrigation service initialized.");
}

void irrigation_start(void){
    if(pump_running){
        xTimerReset(pump_timer, pdMS_TO_TICKS(100));
        ESP_LOGW(TAG, "PUMP RUNNING");
        return;
    }
    relay_set(true);
    led_set(true);
    pump_running = true;
    xTimerStart(pump_timer,pdMS_TO_TICKS(100));
    ESP_LOGW(TAG, "PUMP ON");
}

void irrigation_stop(void){
    if(!pump_running){
        return;
    }
    relay_set(false);
    led_set(false);
    pump_running = false;
    if(xTimerIsTimerActive(pump_timer) == pdTRUE){
        xTimerStop(pump_timer,pdMS_TO_TICKS(100));
    }
    ESP_LOGW(TAG, "PUMP OFF");
}

bool irrigation_is_running(void){
    return pump_running;
}

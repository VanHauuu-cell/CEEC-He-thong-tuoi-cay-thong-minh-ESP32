#include "alert.h"
#include "config.h"
#include "led.h"
#include "buzzer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "ALERT";  // khai bao de xai ESP_LOGW

#define Blink_time 500
static TimerHandle_t blink_timer = NULL;
static bool           led_state = false;
static bool           alert_active = false;

static void blink_timer_cb(TimerHandle_t timer){
    (void)timer;
    led_state = !led_state;
    led_set(led_state);
    buzzer_set(led_state);
}

void alert_init(void){
    blink_timer = xTimerCreate(
        "blink_tmr",
        pdMS_TO_TICKS(Blink_time),
        pdTRUE,   /* auto-reload */
        NULL,
        blink_timer_cb
    );
    configASSERT(blink_timer != NULL);
    led_set(false);
    buzzer_set(false);
}

void alert_on(void){
    if(alert_active) return;
    alert_active = true;
    xTimerStart(blink_timer, pdMS_TO_TICKS(100)); // k phai nhap nhay 100ms dau nha ma doi 100ms neu rtos dang ban viec khac
    ESP_LOGW(TAG, "ALERT ON");
}

void alert_off(void){
    if(!alert_active) return;
    alert_active = false;
    xTimerStop(blink_timer,pdMS_TO_TICKS(100));
    led_set(false);
    buzzer_set(false);
    ESP_LOGW(TAG, "ALERT OFF BROOOO ");
}

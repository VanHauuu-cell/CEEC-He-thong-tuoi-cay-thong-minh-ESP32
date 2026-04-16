#include "alert.h"
#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "ALERT";  // khai bao de xai ESP_LOGW

#define Blink_time 500
static TimerHandle_t blink_timer = NULL;
static int           led_state = 0;
static bool          alert_active = false;

static void blink_timer_cb(TimerHandle_t timer){
    (void)timer;
    led_state = !led_state;
    gpio_set_level(LED_PIN, led_state);
    gpio_set_level(BUZZER_PIN, led_state);
}

void alert_init(void){
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUZZER_PIN)
                      | (1ULL << LED_PIN),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    gpio_set_level(BUZZER_PIN,0);
    gpio_set_level(LED_PIN,0);
    blink_timer = xTimerCreate(
        "blink_tmr",
        pdMS_TO_TICKS(Blink_time),
        pdTRUE,   /* auto-reload */
        NULL,
        blink_timer_cb
    );
    configASSERT(blink_timer != NULL);
    gpio_set_level(LED_PIN, 1);
}

void alert_on(void){
    if(alert_active) return;
    alert_active = true;
    gpio_set_level(LED_PIN,0);
    led_state = 0;
    xTimerStart(blink_timer, pdMS_TO_TICKS(100)); // k phai nhap nhay 100ms dau nha ma doi 100ms neu rtos dang ban viec khac
    ESP_LOGW(TAG, "ALERT ON");
}

void alert_off(void){
    if(!alert_active) return;
    alert_active = false;
    xTimerStop(blink_timer,pdMS_TO_TICKS(100));
    gpio_set_level(LED_PIN, 1);
    gpio_set_level(BUZZER_PIN, 0);
    ESP_LOGW(TAG, "ALERT OFF BROOOO ");
}

void alert_status_led(int on){
    gpio_set_level(LED_PIN, on ? 1 : 0);
}
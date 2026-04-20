#include "fsm.h"
#include "irrigation.h"
#include "sensor.h"
#include "config.h"
#include "alert.h"
#include "relay.h"
#include "rtc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_log.h"

static const char *TAG  = "MAIN";

static void rtc_task(void *pv){
    (void)pv;
    int h = 0, m = 0;
    bool triggered = false;
    while(1) {
        rtc_get_time(&h, &m);
        ESP_LOGI(TAG, "RTC task create, time =%02d : %02d", h, m);
        if(h == 6 && m == 0){
             if(!triggered){
                triggered = true;
                system_event ev = { .event_type = E_RTC_TRIGGER};
                xQueueSend(system_queue, &ev, pdMS_TO_TICKS(100));
                ESP_LOGI(TAG, "RTC trigger sent, time: %02d:%02d", h, m);
             }
        }
        else {
        triggered = false;
        }
     vTaskDelay(pdMS_TO_TICKS(1000));
    }   
}
void app_main(void){
    fsm_init();
    ds3231_init();
    sensor_init();
    irrigation_init();
    alert_init();
    rtc_set_time(21, 0, 0);
    xTaskCreate(fsm_task, "fsm_task", 4096, NULL, 6, NULL);
    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 5, NULL);
    xTaskCreate(rtc_task,    "rtc_task",    2048, NULL, 4, NULL);
}
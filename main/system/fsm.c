#include "fsm.h"
#include "config.h"
#include "irrigation.h"
#include "alert.h"
#include "rtc.h"
#include "wifi_manager.h"
#include "weather_api.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_log.h"

static const char *TAG = "FSM";
QueueHandle_t system_queue = NULL;

system_state current_state = S_IDLE;
float last_temp = 0;
float last_hum = 0;
float last_soil = 0;
bool schedule = false;

static TimerHandle_t wait_timer = NULL;
static void wait_timer_cb(TimerHandle_t timer){
    (void)timer;
    system_event ev = {
        .event_type = E_WAIT_DONE
    };
    xQueueSend(system_queue, &ev, 0);
}


static void enter_idle (void){
    current_state  = S_IDLE;
    ESP_LOGI(TAG, ": IDLE");
}

static void enter_wattering(void){
    current_state = S_WATERRING;
    irrigation_start();
    ESP_LOGI(TAG, ": WATTER (soil=%2.f)", last_soil);
}

static void enter_alert(void){
    current_state = S_ALERT;
    irrigation_stop();
    alert_on();
    ESP_LOGW(TAG, ": ALERT (temp=%2.f)  (hum=%2.f)", last_temp, last_hum);
}

static void enter_error(uint8_t code){
    current_state = S_ERROR;
    irrigation_stop();
    alert_on();
    ESP_LOGI(TAG, ": ERROR (code=0x%02X)", code);
}

static void enter_waiting(void)
{
    current_state = S_WAITING;
    xTimerStart(wait_timer, pdMS_TO_TICKS(100));
    ESP_LOGI(TAG, ": WAITING (%d ms)", WAITING_DURATION_MS);
}

extern bool is_raining = false;

void fsm_handle_event (system_event *ev){
    if(ev-> event_type == E_ERROR){
        enter_error(ev->error_code);
        return;
    }

    if(ev->event_type == E_WEATHER_UPDATE){
        is_raining = ev->weather;
        return;
    }

    if(ev-> event_type == E_BUTTON_PRESS){
        if(current_state != S_WATERRING){
            enter_wattering();
        }
        else{
            irrigation_stop();
            enter_waiting();
        }
        return;
    }

    if(ev-> event_type == E_SENSOR_UPDATE){
        last_temp = ev->temp;
        last_hum = ev->hum;
        last_soil = ev->soil;
    }

    if(ev->event_type == E_RTC_TRIGGER) {
         if(is_raining) {
             ESP_LOGW(TAG, "RTC Trigger received: SKIPPING watering due to rain.");
             return; 
         }
    }
    switch (current_state){
        case S_IDLE:
            if(ev -> event_type == E_RTC_TRIGGER && last_soil > SOIL_WET_THRESHOLD){
                enter_wattering();
                break; 
            }
            if(ev-> event_type == E_SENSOR_UPDATE){
                if(last_hum < HUM_ALERT_LOW && last_temp > TEMP_ALERT_HIGH){
                 enter_alert();
                }
                else if(last_soil > SOIL_DRY_THRESHOLD){
                enter_wattering(); 
                }
            }
            break;

        case S_WATERRING:
            if(ev-> event_type == E_SENSOR_UPDATE){
                last_temp = ev->temp;
                last_hum = ev->hum;
                last_soil = ev->soil;

                if(last_soil < SOIL_WET_THRESHOLD){
                    irrigation_stop();
                    enter_waiting();
                }
            }
            else if(ev-> event_type == E_PUMP_DONE){
                irrigation_stop();
                enter_waiting();
            }
            break;

        case S_WAITING:
            if(ev->event_type == E_WAIT_DONE){
                enter_idle();
            }
            break;
        
        case S_ALERT:
            if(ev ->event_type == E_SENSOR_UPDATE){
                if(last_temp <= TEMP_ALERT_RECOVER || last_hum >= HUM_ALERT_LOW ){
                    alert_off();
                    enter_idle();
                }
            }
            break;
        
        case S_ERROR:
        if(ev->event_type == E_SENSOR_UPDATE){
                ESP_LOGI(TAG, "Sensor recovered, clearing error state");
                if(last_hum < HUM_ALERT_LOW && last_temp > TEMP_ALERT_HIGH) {
                    enter_alert(); 
                } 
                else {
                    alert_off();  
                    enter_idle(); 
                }
            }
            break;

        default:
            enter_idle();
            break;
    }
        
} 

void fsm_init(void){
    current_state = S_IDLE;
    system_queue = xQueueCreate(EVENT_QUEUE_SIZE, sizeof(system_event));
    configASSERT(system_queue != NULL);

    wait_timer = xTimerCreate (
        "wait timer",
        pdMS_TO_TICKS(WAITING_DURATION_MS),
        pdFALSE,
        NULL,
        wait_timer_cb
    );
    configASSERT(wait_timer != NULL);
    ESP_LOGI(TAG, "FSM init");
}

void fsm_task(void *pvParameters){
    (void)pvParameters;
    system_event ev;
    ESP_LOGI(TAG, "FSM task start");
    while(1){
        if(xQueueReceive(system_queue, &ev,  portMAX_DELAY) == pdTRUE){
            fsm_handle_event(&ev);
        }
    }
}


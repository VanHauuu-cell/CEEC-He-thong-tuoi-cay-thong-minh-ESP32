#include "sensor.h"
#include "dht.h"
#include "soil.h"
#include "config.h"
#include "fsm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"

static const char *TAG = "SENSOR";

void sensor_init(void){
    soil_sensor_init();
}

int sensor_read(sensor_data *val){
    float raw_temp = 0;
    float raw_hum = 0;
    bool dht_oke = dht11_read(&raw_temp, &raw_hum);
    if(!dht_oke){
        ESP_LOGW(TAG, "dht has failed");
        return 0;
    }

    val->temp = raw_temp;
    val->hum  = raw_hum;
    val->soil = soil_sensor_read();
    return 1;
}

void sensor_task(void *pvParameters){
    (void)pvParameters;
    ESP_LOGW(TAG, "sensor task start");
    sensor_data data;

    while(1){
        vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_INTERVAL));
        int ret = sensor_read(&data);
        if(!ret){
            system_event err_ev = {
            .event_type = E_ERROR,
            .error_code = ERR_SENSOR_DHT,
        };
        xQueueSend(system_queue, &err_ev, pdMS_TO_TICKS(100));
        continue;
    }

    system_event ev = {
        .event_type = E_SENSOR_UPDATE,
        .temp = data.temp,
        .hum = data.hum,
        .soil = data.soil,
        };

    xQueueSend(system_queue, &ev, pdMS_TO_TICKS(100));
    ESP_LOGI(TAG, "Sent: temp=%.1f*C  hum=%.1f,  soil=%.1f",
         data.temp, data.hum, data.soil);
    }
}
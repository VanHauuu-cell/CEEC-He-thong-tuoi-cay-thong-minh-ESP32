#include <stdio.h>
#include <stdio.h>
#include "sensor.h"
#include "irrigation.h"
#include "freertos/FreeRTOS.h"

void app_main(void)
{
 while (1)
    {
        float temp, soil;

        sensor_read_all(&temp, &soil);
        irrigation_update(temp, soil);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

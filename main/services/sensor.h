#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H
#include <stdint.h>

struct{
    float temp;
    float hum;
    float soil;
} typedef sensor_data;

void sensor_init(void);
int sensor_read(sensor_data *val);
void sensor_task(void *pvParamter);

#endif
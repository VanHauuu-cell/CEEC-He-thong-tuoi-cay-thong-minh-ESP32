#include "sensor.h"
#include "dht.h"
#include "soil.h"

void sensor_read_all(float *temp, float *soil)
{
    *temp = dht_read_temp();
    *soil = soil_read_moisture();
}
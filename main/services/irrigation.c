#include "irrigation.h"
#include "relay.h"

#define SOIL_THRESHOLD 30

void irrigation_update(float temp, float soil)
{
    if (soil < SOIL_THRESHOLD)
    {
        relay_on();
    }
    else
    {
        relay_off();
    }
}
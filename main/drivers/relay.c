#include "relay.h"
#include "driver/gpio.h"

#define RELAY_PIN 2

void relay_on()
{
    gpio_set_level(RELAY_PIN, 1);
}

void relay_off()
{
    gpio_set_level(RELAY_PIN, 0);
}
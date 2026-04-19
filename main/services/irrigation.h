#ifndef IRRIGATION_SERVICE_H
#define IRRIGATION_SERVICE_H

#include <stdbool.h>

void irrigation_update(float temp, float soil);
void irrigation_start(void);
void irrigation_stop(void);
bool irrigation_is_running(void);

#endif
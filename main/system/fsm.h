#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef enum{
    E_INTI_DONE,
    E_WEATHER_UPDATE,
    E_WAIT_DONE,
    E_RTC_TRIGGER,
    E_SENSOR_UPDATE,
    E_PUMP_DONE,
    E_ERROR
} event_type;

typedef enum{
    S_IDLE,
    S_WATERRING,
    S_ALERT,
    S_SCHEDULE
} system_state;

typedef struct{
    event_type event_type;
    float temp;
    float soil;
    float hum;
    bool weather;
    uint8_t error_code;
} system_event;

typedef struct {
    system_state current_state;
    system_state prev_state;     
    float last_temp;
    float last_humidity;
    float last_soil;
    bool  rain_today;         
    bool  schedule_triggered;
    TickType_t pump_start_tick;
} fsm_context;

typedef enum {
    ERR_NONE         = 0,
    ERR_SENSOR_DHT   = 0x01, 
    ERR_SENSOR_SOIL  = 0x02,  
    ERR_RTC_READ     = 0x03,  
    ERR_RELAY_FAULT  = 0x04, 
} error_code;

extern QueueHandle_t system_queue;

void fsm_init(void);
system_state get_state(void);
void fsm_update(system_event *event);
void fsm_task (void *pvParameters);

#endif
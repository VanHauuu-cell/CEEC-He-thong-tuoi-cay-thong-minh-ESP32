#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h" 

#define TEMP_ALERT_HIGH       31.0f   
#define TEMP_ALERT_RECOVER    28.0f 
#define HUM_ALERT_LOW         20.0f   
#define SOIL_DRY_THRESHOLD    2300    
#define SOIL_WET_THRESHOLD    1500   
#define WAITING_DURATION_MS   30000   
#define EVENT_QUEUE_SIZE      10

typedef enum{
    E_INTI_DONE,
    E_WAIT_DONE,
    E_RTC_TRIGGER,
    E_SENSOR_UPDATE,
    E_PUMP_DONE,
    E_BUTTON_PRESS,
    E_WEATHER_UPDATE,
    E_ERROR
} event_type;

typedef enum{
    S_IDLE,
    S_WATERRING,
    S_ALERT,
    S_ERROR,
    S_WAITING
} system_state;

typedef struct{
    event_type event_type;
    float temp;
    float soil;
    float hum;
    bool weather; // true nếu trời mưa, false nếu không mưa
    uint8_t error_code;
} system_event;

typedef enum {
    ERR_NONE         = 0,
    ERR_SENSOR_DHT   = 0x01, 
    ERR_SENSOR_SOIL  = 0x02,  
    ERR_RTC_READ     = 0x03,  
    ERR_RELAY_FAULT  = 0x04, 
} error_code;

extern QueueHandle_t system_queue;

void fsm_init(void);
void fsm_handle_event(system_event * ev);
void fsm_task (void *pvParameters);

#endif
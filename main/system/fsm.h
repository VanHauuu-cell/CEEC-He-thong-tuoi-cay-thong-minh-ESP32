#ifndef FSM_H
#define FSM_H

typedef enum{
    E_RTC_TRIGGER,
    E_SENSOR_UPDATE,
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
} system_event;

void fsm_init(void);
void fsm_update(system_event *event);
system_state get_state(void);

#endif
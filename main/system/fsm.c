#include "config.h"
#include "fsm.h"
#include "irrigation.h"
#include "alert.h"

static system_state current_state;

//init
void fsm_inti(void){
    current_state = S_IDLE;
}

system_state get_state(void){
    return current_state;
}

void state_update(system_event *event){
    switch(current_state){
        case S_IDLE:
            if(event->event_type == E_SENSOR_UPDATE){
                if(event->soil < 2000){
                    irrigation_start();
                    current_state = S_WATERRING;
                }
                else if (event->temp > 35){
                    alert_on();
                    current_state = S_ALERT;
                }
            }
            else if( event->event_type == E_RTC_TRIGGER){
                irrigation_start();
                current_state = S_SCHEDULE;
            }
            break;
        case S_WATERRING:
            if(event->event_type == E_SENSOR_UPDATE){
                if(event->soil >= 2000){
                    irrigation_stop();
                    current_state = S_IDLE;
                }
            }
            break;
        case S_SCHEDULE:
            if(event->event_type == E_SENSOR_UPDATE){
                if(event->soil >= 2000){
                    irrigation_stop();
                    current_state = S_IDLE;
                }
            }
            break;
        case S_ALERT:
            if(event->event_type == E_SENSOR_UPDATE){
                if(event->temp <= 30){
                    alert_off();
                    current_state = S_IDLE;
                }
            }
            break;
    }
}

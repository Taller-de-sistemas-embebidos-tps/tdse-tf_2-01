#ifndef __TASK_INC_TASK_ACTUATOR_ATTRIBUTE_H__
#define __TASK_INC_TASK_ACTUATOR_ATTRIBUTE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum {
    ID_LED_BLUETOOTH,
    ID_LED_KID,
    ID_LED_ADULT,
    ID_LED_ALARM,
    ID_BUZZER
} task_actuator_id_t;

typedef enum task_actuator_ev {
    // EV_ACT_NONE = 0,
    EV_ACT_ON,
    EV_ACT_OFF,
    EV_ACT_BLINK,
    EV_ACT_PULSE
} task_actuator_ev_t;

typedef enum {
    ST_ACT_OFF,
    ST_ACT_ON,
    ST_ACT_BLINK,
    ST_ACT_PULSE,
} task_actuator_st_t;

typedef struct task_actuator_dta {
    task_actuator_ev_t event;
    task_actuator_st_t state;
    task_actuator_st_t prev_state;
    uint32_t tick;
} task_actuator_dta_t;

extern task_actuator_dta_t task_actuator_dta_list[];

typedef struct task_actuator_cfg {
	task_actuator_id_t	identifier;
	GPIO_TypeDef *		gpio_port;
	uint16_t			pin;
	GPIO_PinState		on;
	GPIO_PinState		off;
	uint32_t			tick_blink;
	uint32_t			tick_pulse;
} task_actuator_cfg_t;

extern task_actuator_cfg_t task_actuator_cfg;

#ifdef __cplusplus
}
#endif

#endif  /* __TASK_INC_TASK_ACTUATOR_ATTRIBUTE_H__ */

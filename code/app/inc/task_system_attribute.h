#ifndef __TASK_INC_TASK_SYSTEM_ATTRIBUTE_H__
#define __TASK_INC_TASK_SYSTEM_ATTRIBUTE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ===================== EVENTOS DEL SISTEMA ===================== */
typedef enum {
    EV_NONE = 0,

    EV_MODE_BTN_PRESSED,
    EV_ALARM_BTN_PRESSED,
	EV_SENSOR_ERROR,
    EV_SAVE_DATA,
	EV_SEVERE_LOW_FR,
    EV_LOW_FR,
    EV_LOW_SO2,
    EV_FC_OUT_OF_RANGE,
    EV_NO_OXYGEN

} system_event_t;

/* ===================== ESTADOS PRINCIPALES ===================== */
typedef enum {
    ST_MAIN = 0,
    ST_SLEEP
} system_state_t;

/* ===================== MODOS INTERNOS ===================== */
typedef enum {
    ST_KID = 0,
    ST_ADULT
} system_mode_t;

/* ===================== CONTEXTO DEL SISTEMA ===================== */
typedef struct {
	system_state_t state;
	system_mode_t  mode;


	uint8_t spo2_min;
	uint8_t rr_min;
	uint8_t rr_severe_min;
	uint8_t hr_min;
	uint8_t hr_max;
    system_state_t current_state;
    system_mode_t  current_mode;

    system_event_t last_event;

    uint32_t tick;
} system_ctx_t;

#ifdef __cplusplus
}
#endif

#endif /* __TASK_INC_TASK_SYSTEM_ATTRIBUTE_H__ */

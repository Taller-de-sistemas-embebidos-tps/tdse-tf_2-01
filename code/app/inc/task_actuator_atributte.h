#ifndef __TASK_INC_ACTUATOR_ATTRIBUTE_H__
#define __TASK_INC_ACTUATOR_ATTRIBUTE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* =========================================================
 * EVENTOS DEL ACTUADOR
 * (vienen desde task_system)
 * ========================================================= */
typedef enum
{
    EV_ACT_NONE = 0,

    /* Bluetooth */
    EV_ACT_BL_PAIRING,
    EV_ACT_BL_CONNECTED,
    EV_ACT_BL_DISCONNECTED,

    /* Mode */
    EV_ACT_SWITCH_MODE,

    /* Alarm */
    EV_ACT_ALARM_ON,
    EV_ACT_ALARM_OFF

} actuator_event_t;

/* =========================================================
 * ESTADOS FSM BLUETOOTH
 * ========================================================= */
typedef enum
{
    ST_ACT_BL_DISCONNECTED = 0,
    ST_ACT_BL_PAIRING,
    ST_ACT_BL_CONNECTED

} act_bl_state_t;

/* =========================================================
 * ESTADOS FSM MODO
 * ========================================================= */
typedef enum
{
    ST_ACT_MODE_KID = 0,
    ST_ACT_MODE_ADULT

} act_mode_state_t;

/* =========================================================
 * ESTADOS FSM ALARMA
 * ========================================================= */
typedef enum
{
    ST_ACT_ALARM_OFF = 0,
    ST_ACT_ALARM_ON

} act_alarm_state_t;

/* =========================================================
 * CONTEXTO DEL ACTUADOR
 * (opcional, útil para debug o testing)
 * ========================================================= */
typedef struct
{
    act_bl_state_t    bl_state;
    act_mode_state_t  mode_state;
    act_alarm_state_t alarm_state;

    uint32_t tick;

} actuator_ctx_t;

void actuator_set_event(actuator_event_t ev);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_INC_ACTUATOR_ATTRIBUTE_H__ */

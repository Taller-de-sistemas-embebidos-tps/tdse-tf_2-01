#include "task_actuator_atributte.h"
#include "stm32f1xx_hal.h"
#include "main.h"

/* =========================================================
 * CONTEXTO INTERNO DEL ACTUADOR
 * ========================================================= */

/* ---------- Estados ---------- */
static act_bl_state_t    bl_state    = ST_ACT_BL_DISCONNECTED;
static act_mode_state_t  mode_state  = ST_ACT_MODE_KID;
static act_alarm_state_t alarm_state = ST_ACT_ALARM_OFF;

/* ---------- Timers ---------- */
static uint32_t alarm_tick = 0;
static uint32_t bl_tick    = 0;

/* =========================================================
 * HARDWARE ABSTRACTION (STUBS)
 * ========================================================= */


/* --- LED BLUETOOTH  */
static void bl_led_on(void)     { HAL_GPIO_WritePin(GPIOB, LED_BL_Pin, GPIO_PIN_SET); }
static void bl_led_off(void)    { HAL_GPIO_WritePin(GPIOB, LED_BL_Pin, GPIO_PIN_RESET); }
static void bl_led_toggle(void) { HAL_GPIO_TogglePin(GPIOB, LED_BL_Pin); }

/* --- LED NIÑO  */
static void led_kid_on(void)    { HAL_GPIO_WritePin(GPIOB, LED_KID_Pin, GPIO_PIN_SET); }
static void led_kid_off(void)   { HAL_GPIO_WritePin(GPIOB, LED_KID_Pin, GPIO_PIN_RESET); }

/* --- LED ADULTO  */
static void led_adult_on(void)  { HAL_GPIO_WritePin(GPIOB, LED_ADULT_Pin, GPIO_PIN_SET); }
static void led_adult_off(void) { HAL_GPIO_WritePin(GPIOB, LED_ADULT_Pin, GPIO_PIN_RESET); }

/* --- LED ALARMA */
static void alarm_led_on(void)  { HAL_GPIO_WritePin(GPIOB, LED_ALARM_Pin, GPIO_PIN_SET); }
static void alarm_led_off(void) { HAL_GPIO_WritePin(GPIOB, LED_ALARM_Pin, GPIO_PIN_RESET); }

/* --- BUZZER */
static void buzzer_on(void)     { HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET); }
static void buzzer_off(void)    { HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET); }
static void buzzer_toggle(void) { HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin); }
/* =========================================================
 * INIT
 * ========================================================= */

void actuator_init(void)
{
    bl_state    = ST_ACT_BL_DISCONNECTED;
    mode_state  = ST_ACT_MODE_KID;
    alarm_state = ST_ACT_ALARM_OFF;

    bl_led_off();
    led_kid_on();
    led_adult_off();
    alarm_led_off();
    buzzer_off();
}

/* =========================================================
 * EVENT INJECTION (desde task_system)
 * ========================================================= */

void actuator_set_event(actuator_event_t ev)
{
    switch (ev)
    {
        /* ---------- Bluetooth ---------- */
        case EV_ACT_BL_PAIRING:
            bl_state = ST_ACT_BL_PAIRING;
            bl_tick = HAL_GetTick();
            break;

        case EV_ACT_BL_CONNECTED:
            bl_state = ST_ACT_BL_CONNECTED;
            break;

        case EV_ACT_BL_DISCONNECTED:
            bl_state = ST_ACT_BL_DISCONNECTED;
            break;

        /* ---------- Mode ---------- */
        case EV_ACT_SWITCH_MODE:
            mode_state = (mode_state == ST_ACT_MODE_KID) ?
                         ST_ACT_MODE_ADULT : ST_ACT_MODE_KID;
            break;

        /* ---------- Alarm ---------- */
        case EV_ACT_ALARM_ON:
            alarm_state = ST_ACT_ALARM_ON;
            alarm_tick = HAL_GetTick();
            break;

        case EV_ACT_ALARM_OFF:
            alarm_state = ST_ACT_ALARM_OFF;
            break;

        default:
            break;
    }
}

/* =========================================================
 * FSM UPDATE (llamar periódicamente)
 * ========================================================= */

void actuator_update(void)
{
    uint32_t now = HAL_GetTick();

    /* =====================================================
     * BLUETOOTH FSM
     * ===================================================== */
    switch (bl_state)
    {
        case ST_ACT_BL_DISCONNECTED:
            bl_led_off();
            break;

        case ST_ACT_BL_PAIRING:
            if ((now - bl_tick) >= 500)
            {
                bl_led_toggle();
                bl_tick = now;
            }
            break;

        case ST_ACT_BL_CONNECTED:
            bl_led_on();
            break;
    }

    /* =====================================================
     * MODE FSM
     * ===================================================== */
    switch (mode_state)
    {
        case ST_ACT_MODE_KID:
            led_kid_on();
            led_adult_off();
            break;

        case ST_ACT_MODE_ADULT:
            led_kid_off();
            led_adult_on();
            break;
    }

    /* =====================================================
     * ALARM FSM
     * ===================================================== */
    switch (alarm_state)
    {
        case ST_ACT_ALARM_OFF:
            alarm_led_off();
            buzzer_off();
            break;

        case ST_ACT_ALARM_ON:
            alarm_led_on();

            if ((now - alarm_tick) >= 2000)
            {
                buzzer_toggle();
                alarm_tick = now;
            }
            break;
    }
}

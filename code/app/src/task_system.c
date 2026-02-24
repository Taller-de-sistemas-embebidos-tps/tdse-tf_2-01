#include "task_system.h"
#include "stm32f1xx_hal.h"
#include "task_actuator_atributte.h"
#include "task_system_attribute.h"
#include "task_comm.h"
#include "memory.h"

/* ===================== CONTEXTO ===================== */

static system_ctx_t ctx;

static uint32_t last_save_tick = 0;
static volatile system_event_t pending_event = EV_NONE;

/* Contadores */
static uint32_t apnea_count = 0;
static uint32_t low_so2_count = 0;
static uint32_t cardiac_out_count = 0;

/* ========================================================= */
/* ===================== FUNCIONES PRIVADAS ================= */
/* ========================================================= */

static void system_set_mode(system_mode_t mode)
{
    ctx.current_mode = mode;

    if (mode == ST_ADULT)
    {
        ctx.spo2_min        = 90;
        ctx.rr_min          = 12;
        ctx.rr_severe_min   = 8;   // límite peligroso adulto
        ctx.hr_min          = 60;
        ctx.hr_max          = 100;
    }
    else
    {
        ctx.spo2_min        = 93;
        ctx.rr_min          = 20;
        ctx.rr_severe_min   = 12;  // límite peligroso niño
        ctx.hr_min          = 80;
        ctx.hr_max          = 130;
    }
}

/* ========================================================= */
/* ===================== API PUBLICA ======================== */
/* ========================================================= */

void task_system_init(void)
{
    ctx.current_state = ST_MAIN;
    ctx.tick = 0;

    system_set_mode(ST_KID);   // Modo por defecto
}

/* --------------------------------------------------------- */
/* Recibe resultados del procesamiento PPG                  */
/* --------------------------------------------------------- */

void task_system_post_ppg(ppg_results_t *res)
{
    if (res == NULL || !res->valid)
    {
        pending_event = EV_SENSOR_ERROR;
        return;
    }

    /* Evaluación con rangos dinámicos */

    if (res->rr < ctx.rr_severe_min)
    {
        pending_event = EV_SEVERE_LOW_FR;
    }
    else if (res->rr < ctx.rr_min)
    {
        pending_event = EV_LOW_FR;
    }
    else if (res->spo2 < ctx.spo2_min)
    {
        pending_event = EV_LOW_SO2;
    }
    else if (res->hr < ctx.hr_min || res->hr > ctx.hr_max)
    {
        pending_event = EV_FC_OUT_OF_RANGE;
    }
    else
    {
        pending_event = EV_NONE;
    }
}

/* --------------------------------------------------------- */
/* Máquina de estados principal                             */
/* --------------------------------------------------------- */

void task_system_update(void)
{
    uint32_t now = HAL_GetTick();
    ctx.tick = now;

    /* Guardado periódico (BAJA PRIORIDAD) */
    if ((now - last_save_tick) >= 5000)
    {

        if (pending_event == EV_NONE)
        {
            pending_event = EV_SAVE_DATA;
            last_save_tick = now; // Reiniciamos el reloj SOLO si logramos enviar el evento
        }
    }

    ctx.last_event = pending_event;

    switch (ctx.current_state)
    {

    /* ===================== ESTADO PRINCIPAL ===================== */

    case ST_MAIN:

        /* Cambio de modo */
        if (pending_event == EV_MODE_BTN_PRESSED)
        {
            if (ctx.current_mode == ST_KID)
                system_set_mode(ST_ADULT);
            else
                system_set_mode(ST_KID);
        }
        actuator_set_event(EV_ACT_SWITCH_MODE);

        /* Apagar alarma manual */
        if (pending_event == EV_ALARM_BTN_PRESSED)
        {
            actuator_set_event(EV_ACT_ALARM_OFF);
        }

        /* Evento SEVERO → ÚNICO que activa alarma */
        else if (pending_event == EV_SEVERE_LOW_FR)
        {
            apnea_count++;
            actuator_set_event(EV_ACT_ALARM_ON);
        }

        /* Eventos no severos → solo registro */
        else if (pending_event == EV_LOW_FR)
        {
            apnea_count++;
        }
        else if (pending_event == EV_LOW_SO2)
        {
            low_so2_count++;
        }
        else if (pending_event == EV_FC_OUT_OF_RANGE)
        {
            cardiac_out_count++;
        }
        else if (pending_event == EV_SAVE_DATA)
                {
                    uint32_t stats_to_save[3] = {apnea_count, low_so2_count, cardiac_out_count};


                    // OJO: En el STM32F103 (ej. 128KB), la página 63 suele ser segura: 0x0801FC00
                    uint32_t flash_address = 0x0801FC00;

                    memory_write(flash_address, stats_to_save, sizeof(stats_to_save));
                }

        break;

    /* ===================== ESTADO SLEEP ===================== */

    case ST_SLEEP:

        if (pending_event == EV_MODE_BTN_PRESSED)
        {
            ctx.current_state = ST_MAIN;
            system_set_mode(ST_KID);
        }

        break;
    }

    pending_event = EV_NONE;
}
/* Esta función permite que archivos externos (como main.c) envíen eventos al sistema */
void task_system_set_event(system_event_t ev)
{
    pending_event = ev;
}

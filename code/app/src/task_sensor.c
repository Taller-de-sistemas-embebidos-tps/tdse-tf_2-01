#include "main.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"
#include "app.h"
#include "task_sensor_attribute.h"
#include "task_system_attribute.h"
#include "task_menu_attribute.h"
#include "task_menu_interface.h"
#include "task_comm.h"
#include "task_system.h"
#include "task_sensor.h"

#include "max30102.h"
#include "ppg_processing.h"

/********************** macros and definitions *******************************/
#define G_TASK_SEN_CNT_INIT         0ul
#define G_TASK_SEN_TICK_CNT_INI     0ul

#define MAX_IDLE_TICK               50ul
#define MAX_READ_TICK               100ul

#define IR_THRESHOLD                50000ul

/********************** internal data declaration ****************************/

const task_sensor_cfg_t task_sensor_cfg_list[] = {
    { MAX_IDLE_TICK, MAX_READ_TICK, EV_SAVE_DATA }
};

#define SENSOR_CFG_QTY  (sizeof(task_sensor_cfg_list)/sizeof(task_sensor_cfg_t))

task_sensor_dta_t task_sensor_dta_list[] = {
    { MAX_IDLE_TICK, ST_IDLE, EV_SEN_FINGER_OUT }
};

#define SENSOR_DTA_QTY  (sizeof(task_sensor_dta_list)/sizeof(task_sensor_dta_t))

/********************** internal functions declaration ***********************/
static void task_sensor_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_sensor  = "Task Sensor";
const char *p_task_sensor_ = "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_sensor_cnt;
volatile uint32_t g_task_sensor_tick_cnt;

extern I2C_HandleTypeDef hi2c1;

/********************** sensor driver instance ******************************/
static max30102_t max30102;

/********************** external functions definition ************************/

void task_sensor_init(void *parameters)
{
    uint32_t index;
    task_sensor_dta_t *p_task_sensor_dta;

    LOGGER_INFO(" ");
    LOGGER_INFO("  %s is running - %s", GET_NAME(task_sensor_init), p_task_sensor);
    LOGGER_INFO("  %s is a %s", GET_NAME(task_sensor), p_task_sensor_);

    /* Init processing */
    ppg_init();

    /* Init sensor */
    MAX30102_Init(&max30102, &hi2c1);

    /* Init counters */
    g_task_sensor_cnt = G_TASK_SEN_CNT_INIT;
    LOGGER_INFO("   %s = %lu", GET_NAME(g_task_sensor_cnt), g_task_sensor_cnt);

    for (index = 0; index < SENSOR_DTA_QTY; index++)
    {
        p_task_sensor_dta = &task_sensor_dta_list[index];

        p_task_sensor_dta->state = ST_IDLE;
        p_task_sensor_dta->event = EV_SEN_FINGER_OUT;
        p_task_sensor_dta->tick  = MAX_IDLE_TICK;

        LOGGER_INFO("   Sensor[%lu] State=%lu Event=%lu",
                    index,
                    (uint32_t)p_task_sensor_dta->state,
                    (uint32_t)p_task_sensor_dta->event);
    }
}

void task_sensor_update(void *parameters)
{
    bool b_time_update_required = false;

    __asm("CPSID i");
    if (G_TASK_SEN_TICK_CNT_INI < g_task_sensor_tick_cnt)
    {
        g_task_sensor_tick_cnt--;
        b_time_update_required = true;
    }
    __asm("CPSIE i");

    while (b_time_update_required)
    {
        g_task_sensor_cnt++;
        task_sensor_statechart();

        __asm("CPSID i");
        if (G_TASK_SEN_TICK_CNT_INI < g_task_sensor_tick_cnt)
        {
            g_task_sensor_tick_cnt--;
            b_time_update_required = true;
        }
        else
        {
            b_time_update_required = false;
        }
        __asm("CPSIE i");
    }
}

static void task_sensor_statechart(void)
{
    uint32_t index;
    uint32_t ir_sample  = 0;
    uint32_t red_sample = 0;

    for (index = 0; index < SENSOR_DTA_QTY; index++)
    {
        const task_sensor_cfg_t *cfg = &task_sensor_cfg_list[index];
        task_sensor_dta_t *dta = &task_sensor_dta_list[index];

        /* Leer sensor real */
        MAX30102_ReadIR(&max30102,&ir_sample);

        /* Detectar dedo */
        dta->event = (ir_sample > IR_THRESHOLD)
                     ? EV_SEN_FINGER_IN
                     : EV_SEN_FINGER_OUT;

        switch (dta->state)
        {
            case ST_IDLE:

                if (EV_SEN_FINGER_IN == dta->event)
                {
                    if (0 == dta->tick)
                    {
                        dta->tick  = cfg->max_read_tick;
                        dta->state = ST_READING;
                    }
                    else
                    {
                        dta->tick--;
                    }
                }
                else
                {
                    dta->tick = cfg->max_idle_tick;
                }
                break;

            case ST_READING:

                if (EV_SEN_FINGER_OUT == dta->event)
                {
                    if (0 == dta->tick)
                    {
                        dta->tick  = cfg->max_idle_tick;
                        dta->state = ST_IDLE;
                    }
                    else
                    {
                        dta->tick--;
                    }
                }
                else
                {
                    dta->tick = cfg->max_read_tick;
                }

                /* Toma de muestra */
                if (cfg->max_read_tick == dta->tick)
                {
                    ppg_add_sample(ir_sample, red_sample);

                    ppg_results_t res;
                    if (ppg_compute(&res))
                    {
                        task_system_post_ppg(&res);
                    }

                    //put_event_task_menu(cfg->signal_read_data);
                }
                break;

            default:
                dta->tick  = cfg->max_idle_tick;
                dta->state = ST_IDLE;
                dta->event = EV_SEN_FINGER_OUT;
                break;
        }
    }
}

/********************** end of file ******************************************/

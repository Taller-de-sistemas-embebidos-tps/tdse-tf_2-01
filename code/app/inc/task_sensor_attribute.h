#ifndef TASK_SENSOR_ATTRIBUTE_H_
#define TASK_SENSOR_ATTRIBUTE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/********************** typedef **********************************************/

/* Estados de la máquina de estados */
typedef enum
{
    ST_IDLE = 0,
    ST_READING
} task_sensor_st_t;

/* Eventos del sensor */
typedef enum
{
    EV_SEN_FINGER_OUT = 0,
    EV_SEN_FINGER_IN
} task_sensor_ev_t;

/* Configuración estática del sensor */
typedef struct
{
    uint32_t max_idle_tick;
    uint32_t max_read_tick;
} task_sensor_cfg_t;

/* Datos dinámicos del sensor */
typedef struct
{
    uint32_t          tick;
    task_sensor_st_t  state;
    task_sensor_ev_t  event;
} task_sensor_dta_t;

#ifdef __cplusplus
}
#endif

#endif /* TASK_SENSOR_ATTRIBUTE_H_ */

#ifndef TASK_SENSOR_H
#define TASK_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************/
#include <stdint.h>
#include "ppg_processing.h"
#include "task_system.h"

/********************** public macros ****************************/
/* (por ahora no hay macros públicas del sensor) */

/********************** public types *****************************/
/* NOTA:
 * Estados, eventos y estructuras del sensor
 * se definen en task_sensor_attribute.h
 */

/********************** public variables *************************/
/* Contador de ejecuciones de la tarea sensor */
extern uint32_t g_task_sensor_cnt;

/* Contador de ticks pendientes (actualizado desde ISR) */
extern volatile uint32_t g_task_sensor_tick_cnt;

/********************** public functions *************************/

/**
 * @brief Inicialización de la tarea Sensor
 *
 * Se encarga de:
 *  - Inicializar estados y eventos
 *  - Inicializar ticks
 *  - Mostrar información de debug
 *
 * @param parameters Parámetros genéricos de inicialización (no usados)
 */
void task_sensor_init(void *parameters);

/**
 * @brief Actualización periódica de la tarea Sensor
 *
 * Debe llamarse desde el scheduler principal.
 * Consume ticks y ejecuta la máquina de estados del sensor
 * de forma no bloqueante.
 *
 * @param parameters Parámetros genéricos de ejecución (no usados)
 */
void task_sensor_update(void *parameters);

#ifdef __cplusplus
}
#endif

#endif /* TASK_SENSOR_H */

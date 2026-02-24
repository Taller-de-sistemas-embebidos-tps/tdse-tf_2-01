#ifndef TASK_SYSTEM_H
#define TASK_SYSTEM_H

#include <stdint.h>
#include <stdbool.h>
#include "ppg_processing.h"
#include "task_system_attribute.h"


/* ===================== API ===================== */

void task_system_init(void);
void task_system_update(void);
void task_system_post_ppg(ppg_results_t *res);
void task_system_set_event(system_event_t ev);

#endif

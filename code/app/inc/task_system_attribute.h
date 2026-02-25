#ifndef __TASK_INC_TASK_SYSTEM_ATTRIBUTE_H__
#define __TASK_INC_TASK_SYSTEM_ATTRIBUTE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef enum task_system_ev {
    EV_SYS_BTN_MODE_PRESSED,
    EV_SYS_BTN_MODE_RELEASED,
    EV_SYS_BTN_PAIRING_PRESSED,
    EV_SYS_BTN_PAIRING_RELEASED,
    EV_SYS_BTN_ALARM_PRESSED,
    EV_SYS_BTN_ALARM_RELEASED,
} task_system_ev_t;

typedef enum task_system_st {
    ST_SYS_INIT,
    ST_SYS_MAIN,
} task_system_st_t;

typedef enum task_system_mode_id {
    ID_KID,
    ID_ADULT
} task_system_mode_id_t;

typedef struct task_system_dta {
    task_system_ev_t event;
    task_system_mode_id_t mode;
    task_system_st_t state;
    bool bluetooth_connected;
    uint32_t parameter;
} task_system_dta_t;

typedef struct {
    uint32_t kid_parameter;
    uint32_t adult_parameter;
} task_system_cfg_t;

extern task_system_dta_t task_system_dta;

#ifdef __cplusplus
}
#endif

#endif  /* __TASK_INC_TASK_SYSTEM_ATTRIBUTE_H__ */

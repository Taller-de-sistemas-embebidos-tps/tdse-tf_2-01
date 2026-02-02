#ifndef __TASK_INC_TASK_SYSTEM_ATTRIBUTE_H__
#define __TASK_INC_TASK_SYSTEM_ATTRIBUTE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum task_system_ev {
    EV_SYS_BTN_MODE_ACTIVE,
    EV_SYS_BTN_MODE_IDLE,
    EV_SYS_BTN_PAIRING_ACTIVE,
    EV_SYS_BTN_PAIRING_IDLE,
    EV_SYS_BTN_ALARM_ACTIVE,
    EV_SYS_BTN_ALARM_IDLE,
} task_system_ev_t;

typedef enum task_system_st {
    ST_SYS_MAIN,
    ST_SYS_SLEEP,
} task_system_st_t;

typedef enum task_system_inner_st {
    ST_KID,
    ST_ADULT
} task_system_inner_st_t;

typedef struct task_system_dta {
    task_system_ev_t event;
    task_system_inner_st_t current_mode;
    task_system_st_t state;
} task_system_dta_t;

extern task_system_dta_t task_system_dta;

#ifdef __cplusplus
}
#endif

#endif  /* __TASK_INC_TASK_SYSTEM_ATTRIBUTE_H__ */

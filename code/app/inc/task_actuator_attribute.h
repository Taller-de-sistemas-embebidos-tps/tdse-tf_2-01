#ifndef __TASK_INC_TASK_ACTUATOR_ATTRIBUTE_H__
#define __TASK_INC_TASK_ACTUATOR_ATTRIBUTE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum task_actuator_ev {
    Aaaa
} task_actuator_ev_t;

typedef enum task_actuator_st {
    Bbbb
} task_actuator_st_t;

typedef enum task_actuator_inner_st {
    Cccc
} task_actuator_inner_st_t;

typedef struct task_actuator_dta {
    task_actuator_ev_t event;
    task_actuator_inner_st_t current_mode;
    task_actuator_st_t state;
} task_actuator_dta_t;

extern task_actuator_dta_t task_actuator_dta;

#ifdef __cplusplus
}
#endif

#endif  /* __TASK_INC_TASK_ACTUATOR_ATTRIBUTE_H__ */

#ifndef __TASK_INC_TASK_COMM_ATTRIBUTE_H__
#define __TASK_INC_TASK_COMM_ATTRIBUTE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum task_comm_ev {
    Aaaa
} task_comm_ev_t;

typedef enum task_comm_st {
    Bbbb
} task_comm_st_t;

typedef enum task_comm_inner_st {
    Cccc
} task_comm_inner_st_t;

typedef struct task_comm_dta {
    task_comm_ev_t event;
    task_comm_inner_st_t current_mode;
    task_comm_st_t state;
} task_comm_dta_t;


extern task_comm_dta_t task_comm_dta;

typedef struct task_comm_cfg {
    int x;
} task_comm_cfg_t;

extern task_comm_cfg_t task_comm_cfg;

#ifdef __cplusplus
}
#endif

#endif  /* __TASK_INC_TASK_COMM_ATTRIBUTE_H__ */

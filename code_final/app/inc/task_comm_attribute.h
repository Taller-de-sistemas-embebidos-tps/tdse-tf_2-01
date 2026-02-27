#ifndef __TASK_INC_TASK_COMM_ATTRIBUTE_H__
#define __TASK_INC_TASK_COMM_ATTRIBUTE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum task_comm_ev {
    EV_COMM_SEND_DATA,
} task_comm_ev_t;

typedef enum task_comm_st {
    ST_COMM_CONNECTED,
    ST_COMM_PAIRING,
    ST_COMM_DISCONNECTED
} task_comm_st_t;


typedef struct task_comm_dta {
    task_comm_ev_t event;
    task_comm_st_t state;
    uint32_t pairing_tick;
} task_comm_dta_t;


extern task_comm_dta_t task_comm_dta;

typedef struct task_comm_cfg {
	GPIO_TypeDef *	usart_tx_port;
	uint16_t		usart_tx_pin;
	GPIO_TypeDef *	usart_rx_port;
	uint16_t		usart_rx_pin;
	GPIO_PinState	bit_on;
	GPIO_PinState	bit_off;
} task_comm_cfg_t;

extern task_comm_cfg_t task_comm_cfg;

#ifdef __cplusplus
}
#endif

#endif  /* __TASK_INC_TASK_COMM_ATTRIBUTE_H__ */

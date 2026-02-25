/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file   : task_comm.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes */
#include <task_system_interface.h>
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_comm_attribute.h"
#include "task_system_attribute.h"
#include "task_menu_attribute.h"

/********************** macros and definitions *******************************/
#define G_TASK_COMM_CNT_INIT		0ul
#define G_TASK_COMM_TICK_CNT_INI	0ul

#define DEL_BTN_XX_MIN				0ul
#define DEL_BTN_XX_MED				25ul
#define DEL_BTN_XX_MAX				50ul

/********************** internal data declaration ****************************/
const task_comm_cfg_t task_comm_cfg_list[] = {
	{
		USART_TX_GPIO_Port, USART_TX_Pin,
		USART_RX_GPIO_Port, USART_RX_Pin,
		1, 0 // TODO Revisar que valores poner ahí
	}
};

#define COMM_CFG_QTY	(sizeof(task_comm_cfg_list)/sizeof(task_comm_cfg_t))

task_comm_dta_t task_comm_dta_list[] = {
	{ST_COMM_DISCONNECTED, EV_COMM_SEND_DATA, 0}
};

#define COMM_DTA_QTY	(sizeof(task_comm_dta_list)/sizeof(task_comm_dta_t))

/********************** internal functions declaration ***********************/
void task_comm_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_comm 		= "Task Comm (Comm Statechart)";
const char *p_task_comm_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_comm_cnt;
volatile uint32_t g_task_comm_tick_cnt;

/********************** external functions definition ************************/
void task_comm_init(void *parameters)
{
	uint32_t index;
	task_comm_dta_t *p_task_comm_dta;
	task_comm_st_t state;
	task_comm_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_comm_init), p_task_comm);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_comm), p_task_comm_);

	/* Init & Print out: Task execution counter */
	g_task_comm_cnt = G_TASK_COMM_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_comm_cnt), g_task_comm_cnt);

	for (index = 0; COMM_DTA_QTY > index; index++)
	{
		/* Update Task comm Data Pointer */
		p_task_comm_dta = &task_comm_dta_list[index];

		/* Init & Print out: Index & Task execution FSM */
		state = ST_COMM_DISCONNECTED;
		p_task_comm_dta->state = state;

		event = 0; // TODO cambiar luego;
		p_task_comm_dta->event = event;

		// LOGGER_INFO(" ");
		// LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
		// 		    GET_NAME(index), index,
		// 			GET_NAME(state), (uint32_t)state,
		// 			GET_NAME(event), (uint32_t)event);
	}
}

void task_comm_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_COMM_TICK_CNT_INI < g_task_comm_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_comm_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_comm_cnt++;

		/* Run Task comm Statechart */
    	task_comm_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_COMM_TICK_CNT_INI < g_task_comm_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_comm_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_comm_statechart(void)
{
	// uint32_t index;
	// const task_comm_cfg_t *p_task_comm_cfg;
	// task_comm_dta_t *p_task_comm_dta;
	//
	switch (task_comm_dta_list[0].event) {
		case EV_COMM_SEND_DATA:
			// HAL_UART_Transmit(&huart1, (uint8_t*)tx_2, size2, 1000);
			break;
	}
}
/********************** end of file ******************************************/

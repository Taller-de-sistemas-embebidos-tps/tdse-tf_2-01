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
 * @file   : task_actuator.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_actuator_attribute.h"
#include "task_actuator_interface.h"

/********************** macros and definitions *******************************/
#define G_TASK_ACT_CNT_INIT			0ul
#define G_TASK_ACT_TICK_CNT_INI		0ul

#define DEL_ACT_PUL				250ul
#define DEL_ACT_BLI				500ul
#define DEL_ACT_MIN				0ul

/********************** internal data declaration ****************************/
const task_actuator_cfg_t task_actuator_cfg_list[] = {
	{ID_LED_BLUETOOTH,  LED_BL_GPIO_Port,  LED_BL_Pin, GPIO_PIN_SET, 
		GPIO_PIN_RESET,  DEL_ACT_BLI, DEL_ACT_PUL},
	{ID_LED_KID,  LED_KID_GPIO_Port,  LED_KID_Pin, GPIO_PIN_SET, 
		GPIO_PIN_RESET,  DEL_ACT_BLI, DEL_ACT_PUL},
	{ID_LED_ADULT,  LED_ADULT_GPIO_Port,  LED_ADULT_Pin, GPIO_PIN_SET, 
		GPIO_PIN_RESET,  DEL_ACT_BLI, DEL_ACT_PUL},
	{ID_LED_ALARM,  LED_ALARM_GPIO_Port,  LED_ALARM_Pin, GPIO_PIN_SET, 
		GPIO_PIN_RESET,  DEL_ACT_BLI, DEL_ACT_PUL},
	{ID_BUZZER,  BUZZER_GPIO_Port,  BUZZER_Pin, GPIO_PIN_SET, 
		GPIO_PIN_RESET,  DEL_ACT_BLI, DEL_ACT_PUL},
};

#define ACTUATOR_CFG_QTY	(sizeof(task_actuator_cfg_list)/sizeof(task_actuator_cfg_t))

task_actuator_dta_t task_actuator_dta_list[] = {
	{EV_ACT_OFF, ST_ACT_OFF, ST_ACT_OFF, 0},
	{EV_ACT_OFF, ST_ACT_OFF, ST_ACT_OFF, 0},
	{EV_ACT_OFF, ST_ACT_OFF, ST_ACT_OFF, 0},
	{EV_ACT_OFF, ST_ACT_OFF, ST_ACT_OFF, 0},
	{EV_ACT_OFF, ST_ACT_OFF, ST_ACT_OFF, 0},
};

#define ACTUATOR_DTA_QTY	(sizeof(task_actuator_dta_list)/sizeof(task_actuator_dta_t))

/********************** internal functions declaration ***********************/
void task_actuator_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_actuator 		= "Task Actuator (Actuator Statechart)";
const char *p_task_actuator_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_actuator_cnt;
volatile uint32_t g_task_actuator_tick_cnt;

/********************** external functions definition ************************/
void task_actuator_init(void *parameters)
{
	uint32_t index;
	const task_actuator_cfg_t *p_task_actuator_cfg;
	task_actuator_dta_t *p_task_actuator_dta;
	task_actuator_st_t state;
	task_actuator_ev_t event;
	bool b_event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_actuator_init), p_task_actuator);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_actuator), p_task_actuator_);

	/* Init & Print out: Task execution counter */
	g_task_actuator_cnt = G_TASK_ACT_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_actuator_cnt), g_task_actuator_cnt);

	for (index = 0; ACTUATOR_DTA_QTY > index; index++)
	{
		/* Update Task Actuator Configuration & Data Pointer */
		p_task_actuator_cfg = &task_actuator_cfg_list[index];
		p_task_actuator_dta = &task_actuator_dta_list[index];

		/* Init & Print out: Index & Task execution FSM */
		state = ST_ACT_OFF;
		p_task_actuator_dta->state = state;

		event = EV_ACT_OFF;
		p_task_actuator_dta->event = event;

		b_event = false;
		// p_task_actuator_dta->flag = b_event;

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu   %s = %s",
					 GET_NAME(index), index,
					 GET_NAME(state), (uint32_t)state,
					 GET_NAME(event), (uint32_t)event,
					 GET_NAME(b_event), (b_event ? "true" : "false"));

		HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->off);
	}
}

void task_actuator_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_ACT_TICK_CNT_INI < g_task_actuator_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_actuator_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_actuator_cnt++;

		/* Run Task Statechart */
    	task_actuator_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_ACT_TICK_CNT_INI < g_task_actuator_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_actuator_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void handle_led(const task_actuator_cfg_t* p_task_actuator_cfg, bool on) {
	HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, on ? p_task_actuator_cfg->on : p_task_actuator_cfg->off);
}

void task_actuator_statechart(void)
{
	uint32_t index;
	const task_actuator_cfg_t *p_task_actuator_cfg;
	task_actuator_dta_t *p_task_actuator_dta;

	for (index = 0; ACTUATOR_DTA_QTY > index; index++)
	{
		/* Update Task Actuator Configuration & Data Pointer */
		p_task_actuator_cfg = &task_actuator_cfg_list[index];
		p_task_actuator_dta = &task_actuator_dta_list[index];

		switch (p_task_actuator_dta->state)
		{
			case ST_ACT_OFF:

				if (EV_ACT_ON == p_task_actuator_dta->event)
				{
					HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->on);
					p_task_actuator_dta->state = ST_ACT_ON;
				} else if (EV_ACT_BLINK == p_task_actuator_dta->event) {
					p_task_actuator_dta->state = ST_ACT_BLINK;
					handle_led(p_task_actuator_cfg, true);
					p_task_actuator_dta->tick = DEL_ACT_BLI;
				} else if (EV_ACT_PULSE == p_task_actuator_dta->event) {
					p_task_actuator_dta->state = ST_ACT_PULSE;
					handle_led(p_task_actuator_cfg, true);
					p_task_actuator_dta->tick = DEL_ACT_PUL;
				}
				break;

			case ST_ACT_ON:

				if (EV_ACT_OFF == p_task_actuator_dta->event)
				{
					HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->off);
					p_task_actuator_dta->state = ST_ACT_OFF;
				}

				break;

			// case ST_ACT_BLINK_ON:
			// 	if (EV_ACT_OFF == p_task_actuator_dta->event) {
			// 		p_task_actuator_dta->state = ST_ACT_BLINK_OFF;
			// 		handle_led(p_task_actuator_cfg, false);
			// 	} else if (EV_ACT_ON == p_task_actuator_dta->event) {
			// 		p_task_actuator_dta->state = ST_ACT_BLINK_ON;
			// 		handle_led(p_task_actuator_cfg, true);
			// 	} else if (p_task_actuator_dta->tick == 0) {
			// 		p_task_actuator_dta->state = ST_ACT_BLINK_OFF;
			// 		handle_led(p_task_actuator_cfg, false);
			// 		p_task_actuator_dta->tick = DEL_ACT_BLI;
			// 	} else {
			// 		(p_task_actuator_dta->tick)--;
			// 	}
			// 	break;
			//
			// case ST_ACT_BLINK_OFF:
			// 	if (EV_ACT_OFF == p_task_actuator_dta->event) {
			// 		p_task_actuator_dta->state = ST_ACT_OFF;
			// 		handle_led(p_task_actuator_cfg, false);
			// 	} else if (EV_ACT_ON == p_task_actuator_dta->event) {
			// 		p_task_actuator_dta->state = ST_ACT_ON;
			// 		handle_led(p_task_actuator_cfg, true);
			// 	} else if (p_task_actuator_dta->tick == 0) {
			// 		p_task_actuator_dta->state = ST_ACT_BLINK_ON;
			// 		handle_led(p_task_actuator_cfg, true);
			// 		p_task_actuator_dta->tick = DEL_ACT_BLI;
			// 	} else {
			// 		(p_task_actuator_dta->tick)--;
			// 	}
			// 	break;

			case ST_ACT_PULSE:
				if (p_task_actuator_dta->tick == 0) {
					p_task_actuator_dta->state = ST_ACT_OFF;
					p_task_actuator_dta->tick = DEL_ACT_PUL;
				} else {
					(p_task_actuator_dta->tick)--;
				}
				break;

			default:

				p_task_actuator_dta->tick  = DEL_ACT_MIN;
				p_task_actuator_dta->state = ST_ACT_OFF;
				p_task_actuator_dta->event = EV_ACT_OFF;
				// p_task_actuator_dta->flag = false;

				break;
		}
	}
}

/********************** end of file ******************************************/

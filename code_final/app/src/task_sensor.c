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
 * @file   : task_sensor.c
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
#include "task_sensor_attribute.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"
#include "max30102.h"
#include "ppg_processing.h"

	/********************** macros and definitions *******************************/
#define G_TASK_SEN_CNT_INIT			0ul
#define G_TASK_SEN_TICK_CNT_INI		0ul

#define DEL_SEN_XX_MIN				0ul
#define DEL_SEN_XX_MED				25ul
#define DEL_SEN_XX_MAX				1000ul

/********************** internal data declaration ****************************/
const task_sensor_cfg_t task_sensor_cfg = {};

task_sensor_dta_t task_sensor_dta = {
	.state = ST_SEN_IDLE,
	.event = EV_SEN_FINGER_OUT,
	.results = {},
	.ppg = {},
	.tick = DEL_SEN_XX_MAX,
};



/********************** internal functions declaration ***********************/
void task_sensor_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_sensor 		= "Task Sensor (Sensor Statechart)";
const char *p_task_sensor_ 		= "Non-Blocking & Update By Time Code";


/********************** external data declaration ****************************/
uint32_t g_task_sensor_cnt;
volatile uint32_t g_task_sensor_tick_cnt;

/********************** external functions definition ************************/
void task_sensor_init(void *parameters)
{
	task_sensor_dta_t *p_task_sensor_dta;
	task_sensor_st_t state;
	task_sensor_ev_t event;

	/* Print out: Task Initialized */
	//LOGGER_INFO(" ");
	//LOGGER_INFO("  %s is running - %s", GET_NAME(task_sensor_init), p_task_sensor);
	//LOGGER_INFO("  %s is a %s", GET_NAME(task_sensor), p_task_sensor_);

	/* Init & Print out: Task execution counter */
	g_task_sensor_cnt = G_TASK_SEN_CNT_INIT;
	//LOGGER_INFO("   %s = %lu", GET_NAME(g_task_sensor_cnt), g_task_sensor_cnt);

	/* Update Task Sensor Data Pointer */
	p_task_sensor_dta = &task_sensor_dta;

	/* Init & Print out: Task execution FSM */
	state = ST_SEN_IDLE;
	p_task_sensor_dta->state = state;

	event = EV_SEN_FINGER_OUT;
	p_task_sensor_dta->event = event;
	//LOGGER_INFO(" ");
	//LOGGER_INFO("   %s = %lu   %s = %lu",
	//			GET_NAME(state), (uint32_t)state,
	//			GET_NAME(event), (uint32_t)event);

	MAX30102_Init(&max30102, &hi2c1);
	ppg_init();
}

void task_sensor_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_SEN_TICK_CNT_INI < g_task_sensor_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_sensor_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_sensor_cnt++;

		/* Run Task Statechart */
    	task_sensor_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_SEN_TICK_CNT_INI < g_task_sensor_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_sensor_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

#define NOT_FINGER_THRESHOLD (80000)

void task_sensor_statechart(void)
{
	const task_sensor_cfg_t *p_task_sensor_cfg;
	task_sensor_dta_t *p_task_sensor_dta;

	p_task_sensor_cfg = &task_sensor_cfg;
	p_task_sensor_dta = &task_sensor_dta;

	uint32_t *red = &(p_task_sensor_dta->red);
	uint32_t *ir = &(p_task_sensor_dta->ir);


	static uint32_t sensor_read_timer = 10;
	bool new_data_available = false;

	if (sensor_read_timer > 0) {
		sensor_read_timer--;
	}
	else {
		sensor_read_timer = 20;
		MAX30102_Read(&max30102, red, ir);
		new_data_available = true;
	}


	if (new_data_available) {
		if (*ir < NOT_FINGER_THRESHOLD && *red < NOT_FINGER_THRESHOLD) {
			p_task_sensor_dta->event = EV_SEN_FINGER_OUT;
		} else {
			p_task_sensor_dta->event = EV_SEN_FINGER_IN;

			if (p_task_sensor_dta->state == ST_SEN_ACTIVE) {
				ppg_add_sample(*ir, *red);
			}
		}
	}

	switch (p_task_sensor_dta->state) {

		case ST_SEN_IDLE:
			if(p_task_sensor_dta->event == EV_SEN_FINGER_IN){
				p_task_sensor_dta->state = ST_SEN_DETECTING;
				p_task_sensor_dta->tick = DEL_SEN_XX_MAX;
			}
			break;

		case ST_SEN_LOSING:

			if(EV_SEN_FINGER_IN == p_task_sensor_dta->event) {
				if(p_task_sensor_dta->tick > DEL_SEN_XX_MIN){
					p_task_sensor_dta->state = ST_SEN_LOSING;
					(p_task_sensor_dta->tick)--;
				}
				else {
					p_task_sensor_dta->state = ST_SEN_ACTIVE;
				}
			}
			else {
				if(p_task_sensor_dta->tick > DEL_SEN_XX_MIN){
					p_task_sensor_dta->state = ST_SEN_LOSING;
					(p_task_sensor_dta->tick)--;
				}
				else {
					p_task_sensor_dta->state = ST_SEN_IDLE;
					//put_event_task_system(p_task_sensor_cfg->signal_down);
				}
			}
			break;

		case ST_SEN_DETECTING:
			if(EV_SEN_FINGER_IN == p_task_sensor_dta-> event) {
				if(p_task_sensor_dta->tick >DEL_SEN_XX_MIN){
					p_task_sensor_dta->state = ST_SEN_DETECTING;
					(p_task_sensor_dta->tick)--;
				}
				else {
					p_task_sensor_dta->state = ST_SEN_ACTIVE;
			//		put_event_task_system(p_task_sensor_cfg->signal_up);
				}
			}
			else {
				if(p_task_sensor_dta->tick > DEL_SEN_XX_MIN){
					p_task_sensor_dta->state = ST_SEN_DETECTING;
					(p_task_sensor_dta->tick)--;
				}
				else {
					p_task_sensor_dta->state = ST_SEN_IDLE;
				}
			}
			break;


		case ST_SEN_ACTIVE:
			if (p_task_sensor_dta->event == EV_SEN_FINGER_OUT) {
				p_task_sensor_dta->state = ST_SEN_LOSING;
				p_task_sensor_dta->tick = DEL_SEN_XX_MAX;
			}
			else {
				if (p_task_sensor_dta->tick == 0) {
					task_sensor_results_dta_t data;

					p_task_sensor_dta->tick = DEL_SEN_XX_MAX;
					//LOGGER_INFO("red %lu , ir %lu", red, ir);
					ppg_results_t *data2 = &(p_task_sensor_dta->ppg);
					ppg_compute(data2);
					data.heart_rate = data2->hr;
					data.respiratory_rate = data2->rr;
					data.spo2 = data2->spo2;
					data.timestamp = g_task_sensor_tick_cnt;
					data.apnea = data2->apnea;
					put_data_task_system(data);

				} else {
					(p_task_sensor_dta->tick)--;
				}
			}
			break;
	}

	//LOGGER_INFO("ESTOY EN SENSOR");


}

/********************** end of file ******************************************/

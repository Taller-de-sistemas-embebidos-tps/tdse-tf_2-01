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
 * @file   : task_test.c
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
#include "app.h"

/********************** macros and definitions *******************************/
#define G_TASK_TEST_CNT_INIT			0ul
#define G_TASK_TEST_TICK_CNT_INI		0ul


/********************** internal data declaration ****************************/
/*
 * const task_test_cfg_t task_test_cfg_list[] = {
	{ID_BTN_MODE,  BTN_MODE_PORT,  BTN_MODE_PIN,  BTN_MODE_PRESSED, DEL_BTN_XX_MAX,
	 EV_SYS_BTN_MODE_IDLE,  EV_SYS_BTN_MODE_ACTIVE},
	{ID_BTN_PAIRING,  BTN_PAIRING_PORT,  BTN_PAIRING_PIN,  BTN_PAIRING_PRESSED, DEL_BTN_XX_MAX,
	 EV_SYS_BTN_PAIRING_IDLE,  EV_SYS_BTN_PAIRING_ACTIVE},
	{ID_BTN_ALARM,  BTN_ALARM_PORT,  BTN_ALARM_PIN,  BTN_ALARM_PRESSED, DEL_BTN_XX_MAX,
	 EV_SYS_BTN_ALARM_IDLE,  EV_SYS_BTN_ALARM_ACTIVE}
};

#define SENSOR_CFG_QTY	(sizeof(task_test_cfg_list)/sizeof(task_test_cfg_t))

task_test_dta_t task_test_dta_list[] = {
	{DEL_BTN_XX_MIN, ST_BTN_XX_UP, EV_BTN_XX_UP},
	{DEL_BTN_XX_MIN, ST_BTN_XX_UP, EV_BTN_XX_UP},
	{DEL_BTN_XX_MIN, ST_BTN_XX_UP, EV_BTN_XX_UP}
};

#define SENSOR_DTA_QTY	(sizeof(task_test_dta_list)/sizeof(task_test_dta_t))
*/

/********************** internal functions declaration ***********************/
void task_test_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_test 		= "Task Test (Test Statechart)";
const char *p_task_test_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_test_cnt;
volatile uint32_t g_task_test_tick_cnt;

/********************** external functions definition ************************/
void task_test_init(void *parameters)
{
	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_test_init), p_task_test);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_test), p_task_test_);

	/* Init & Print out: Task execution counter */
	g_task_test_cnt = G_TASK_TEST_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_test_cnt), g_task_test_cnt);

	// LOGGER_INFO(" ");
	// LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
	// 			GET_NAME(index), index,
	// 			GET_NAME(state), (uint32_t)state,
	// 			GET_NAME(event), (uint32_t)event);
}

void task_test_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_TEST_TICK_CNT_INI < g_task_test_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_test_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_test_cnt++;

		/* Run Task Sensor Statechart */
    	task_test_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_TEST_TICK_CNT_INI < g_task_test_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_test_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_test_statechart(void)
{
	// uint32_t index;
	// const task_test_cfg_t *p_task_test_cfg;
	// task_test_dta_t *p_task_test_dta;
}
/********************** end of file ******************************************/

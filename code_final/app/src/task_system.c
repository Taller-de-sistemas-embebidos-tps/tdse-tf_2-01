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
 * @file   : task_system.c
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
#include "task_system_attribute.h"
#include "task_system_interface.h"
#include "task_actuator_attribute.h"
#include "task_actuator_interface.h"
#include "display.h"
#include "comm.h"

extern UART_HandleTypeDef huart2;

/********************** macros and definitions *******************************/
#define G_TASK_SYS_CNT_INI			0ul
#define G_TASK_SYS_TICK_CNT_INI		0ul

#define DEL_SYS_MIN					0ul
#define DEL_SYS_MED					25ul
#define DEL_SYS_MAX					50ul

#define SPO2_LOW				86

#define HR_LOW_KID_THRESHOLD	90
#define HR_HIGH_KID_THRESHOLD	120
#define RR_LOW_KID_THRESHOLD	15
#define RR_HIGH_KID_THRESHOLD	30

#define HR_LOW_ADULT_THRESHOLD	45
#define HR_HIGH_ADULT_THRESHOLD	90
#define RR_LOW_ADULT_THRESHOLD	10
#define RR_HIGH_ADULT_THRESHOLD	20

/********************** internal data declaration ****************************/
const parameters_t kid_parameters  = {
	.spo2 = SPO2_LOW,
	.min_hr = HR_LOW_KID_THRESHOLD,
	.max_hr = HR_HIGH_KID_THRESHOLD,
	.min_rr = RR_LOW_KID_THRESHOLD,
	.max_rr = RR_HIGH_KID_THRESHOLD,
};

const parameters_t adult_parameters = {
	.spo2 = SPO2_LOW,
	.min_hr = HR_LOW_ADULT_THRESHOLD,
	.max_hr = HR_HIGH_ADULT_THRESHOLD,
	.min_rr = RR_LOW_ADULT_THRESHOLD,
	.max_rr = RR_HIGH_ADULT_THRESHOLD,
};

task_system_dta_t task_system_dta =	{
	ST_SYS_MAIN,
	EV_SYS_BTN_PAIRING_PRESSED,
	ID_KID,
	false,
	800,
	kid_parameters,
};

task_system_cfg_t task_system_cfg = { 100, 200 };

#define SYSTEM_DTA_QTY	(sizeof(task_system_dta)/sizeof(task_system_dta_t))

/********************** internal functions declaration ***********************/
void task_system_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_system 		= "Task System (System Statechart)";
const char *p_task_system_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_system_cnt;
volatile uint32_t g_task_system_tick_cnt;

/********************** external functions definition ************************/
void task_system_init(void *parameters)
{
	task_system_dta_t 	*p_task_system_dta;
	task_system_st_t	state;
	task_system_ev_t	event;
	bool b_event;

	/* Print out: Task Initialized */
	//LOGGER_INFO(" ");
	//LOGGER_INFO("  %s is running - %s", GET_NAME(task_system_init), p_task_system);
	//LOGGER_INFO("  %s is a %s", GET_NAME(task_system), p_task_system_);

	/* Init & Print out: Task execution counter */
	g_task_system_cnt = G_TASK_SYS_CNT_INI;
	//LOGGER_INFO("   %s = %lu", GET_NAME(g_task_system_cnt), g_task_system_cnt);

	init_queue_event_task_system();

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_system_dta = &task_system_dta;

	/* Init & Print out: Task execution FSM */
	state = ST_SYS_MAIN;
	p_task_system_dta->state = state;

	event = EV_SYS_BTN_PAIRING_PRESSED;
	p_task_system_dta->event = event;

	b_event = false;
	// p_task_system_dta->flag = b_event;

	//LOGGER_INFO(" ");
	//LOGGER_INFO("   %s = %lu   %s = %lu   %s = %s",
	//			 GET_NAME(state), (uint32_t)state,
	//			 GET_NAME(event), (uint32_t)event,
	//			 GET_NAME(b_event), (b_event ? "true" : "false"));

	displayInit( DISPLAY_CONNECTION_GPIO_4BITS );

    displayCharPositionWrite(0, 0);
	displayStringWrite("Sleep Centinel");

	hm10_init(&huart2);
	/*uint8_t buffer[10] = {0};
	if (hm10_receive_buffer(buffer, 2, 200) == HAL_OK) {
	    LOGGER_INFO("HM10 responde: %s", buffer);
	} else {
	    LOGGER_INFO("HM10 no responde: %s", buffer);
	}*/

}

void task_system_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_SYS_TICK_CNT_INI < g_task_system_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_system_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_system_cnt++;

		/* Run Task Statechart */
    	task_system_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_SYS_TICK_CNT_INI < g_task_system_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_system_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}
void format_to_lcd_string(char out[2][17], task_sensor_results_dta_t data);

void task_system_statechart(void)
{
	task_system_dta_t *p_task_system_dta;
	task_system_cfg_t *p_task_system_cfg;

	/* Update Task System Data Pointer */
	p_task_system_dta = &task_system_dta;
	p_task_system_cfg = &task_system_cfg;


	if (any_sensor_results()) {
			// NUEVO 1: Agregamos 'static' para que la memoria no se borre
			// mientras el UART envía los datos en segundo plano.
			static char lcd_text[2][17];

			task_sensor_results_dta_t result = get_sensor_results();
			format_to_lcd_string(lcd_text, result);

			displayCharPositionWrite(0, 0);
			displayStringWrite(lcd_text[0]);
			displayCharPositionWrite(0, 1);
			displayStringWrite(lcd_text[1]);

			// NUEVO 2: Usamos la versión no bloqueante por interrupción (_IT)
			// Le quitamos el '500' del final porque ya no hay timeout de espera.
			HAL_UART_Transmit_IT(&huart1, (uint8_t *)lcd_text, 17*2);

			parameters_t params = p_task_system_dta->parameters;
		if (result.spo2 < params.spo2)
			put_event_task_actuator(EV_ACT_ON, ID_LED_ALARM);
		if (result.heart_rate < params.min_hr || result.heart_rate > params.max_hr) {
			put_event_task_actuator(EV_ACT_ON, ID_LED_ALARM);
		}
		if (result.respiratory_rate < params.min_rr || result.respiratory_rate > params.max_rr)
		{
			put_event_task_actuator(EV_ACT_ON, ID_LED_ALARM);
		}
		if (result.apnea == true) {
			put_event_task_actuator(EV_ACT_ON, ID_LED_ALARM);
			put_event_task_actuator(EV_ACT_ON, ID_BUZZER);
		}
	}


	if (true == any_event_task_system())
	{
		// p_task_system_dta->flag = true;
		p_task_system_dta->event = get_event_task_system();


		switch (p_task_system_dta->state)
		{
			case ST_SYS_MAIN:
				switch (p_task_system_dta->event) {
					case EV_SYS_BTN_MODE_PRESSED:
						//LOGGER_INFO("cambiando de estado");
						if (p_task_system_dta->mode == ID_KID) {
							p_task_system_dta->mode = ID_ADULT;
							p_task_system_dta->parameters = adult_parameters;
							put_event_task_actuator(EV_ACT_OFF, ID_LED_KID);
							put_event_task_actuator(EV_ACT_ON, ID_LED_ADULT);
							displayCharPositionWrite(0, 1);
							displayStringWrite("           ");
							displayCharPositionWrite(0, 1);
							displayStringWrite("Modo adulto");
						} else {
							p_task_system_dta->mode = ID_KID;
							p_task_system_dta->parameters = kid_parameters;
							put_event_task_actuator(EV_ACT_OFF, ID_LED_ADULT);
							put_event_task_actuator(EV_ACT_ON, ID_LED_KID);
							displayCharPositionWrite(0, 1);
							displayStringWrite("           ");
							displayCharPositionWrite(0, 1);
							displayStringWrite("Modo nino");
						}
						break;

					case EV_SYS_BTN_ALARM_PRESSED:
						put_event_task_actuator(EV_ACT_OFF, ID_LED_ALARM);
						put_event_task_actuator(EV_ACT_OFF, ID_BUZZER);
						//LOGGER_INFO("PRENDIENDO_LA_ALARMA");
						// displayCharPositionWrite(0, 1);
						// displayStringWrite("ALARMA");
						break;

					case EV_SYS_BTN_PAIRING_PRESSED:
						put_event_task_actuator(EV_ACT_BLINK, ID_LED_BLUETOOTH);
						// displayStringWrite("PAIR");
						break;

					default:
						//LOGGER_INFO("NO ESTOY EN NINGUN CASO DEL MAIN");
						// displayStringWrite("NONE");
						break;
				}

				break;

			default:

				// p_task_system_dta->tick  = DEL_SYS_MIN;
				// p_task_system_dta->state = ST_SYS_IDLE;
				// p_task_system_dta->event = EV_SYS_IDLE;
				// p_task_system_dta->flag = false;
				break;
		}
	}
}

void format_to_lcd_string(char out[2][17], task_sensor_results_dta_t data)
{
    // Línea 1
    out[0][0] = 'a';
    out[0][1] = 'p';
    out[0][2] = 'n';
    out[0][3] = 'e';
    out[0][4] = 'a';
    out[0][5] = ' ';

    if(data.apnea) {
        out[0][6] = 'S';
        out[0][7] = 'I';
    } else {
        out[0][6] = 'N';
        out[0][7] = 'O';
    }

    out[0][8]  = ' ';
    out[0][9]  = ' ';
    out[0][10] = 'h';
    out[0][11] = 'r';
    out[0][12] = ' ';

    out[0][13] = '0' + (data.heart_rate / 100) % 10;
    out[0][14] = '0' + (data.heart_rate / 10) % 10;
    out[0][15] = '0' + (data.heart_rate % 10);

    out[0][16] = '\0';

    // Línea 2
    out[1][0] = 's';
    out[1][1] = 'p';
    out[1][2] = 'o';
    out[1][3] = '2';
    out[1][4] = ' ';

    out[1][5] = '0' + (data.spo2 / 100) % 10;
    out[1][6] = '0' + (data.spo2 / 10) % 10;
    out[1][7] = '0' + (data.spo2 % 10);

    out[1][8]  = ' ';
    out[1][9]  = ' ';
    out[1][10] = 'r';
    out[1][11] = 'r';
    out[1][12] = ' ';

    out[1][13] = '0' + (data.respiratory_rate / 100) % 10;
    out[1][14] = '0' + (data.respiratory_rate / 10) % 10;
    out[1][15] = '0' + (data.respiratory_rate % 10);

    out[1][16] = '\0';
}

/********************** end of file ******************************************/

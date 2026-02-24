#include "task_comm.h"
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart1;   // Ajustar al UART que uses

static bool bt_connected = false;

void bt_comm_init(void)
{
    bt_connected = false;
}

void bt_comm_set_connected(bool state)
{
    bt_connected = state;
}

void bt_comm_send_data(bt_data_t *data)
{
    if (!bt_connected || data == NULL)
        return;

    char buffer[200];

    snprintf(buffer, sizeof(buffer),
        "Respiraciones por minutos= %lu; "
        "Apneas=%lu; "
        "SPO2 = %lu%%; "
        "LOW_SPO2=%lu; "
        "Palpitaciones por minuto= %lu; "
        "HR_ISSUES=%lu\r\n",
        data->resp_rate,
        data->apneas,
        data->spo2,
        data->low_spo2_events,
        data->heart_rate,
        data->hr_issues
    );

    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}

void bt_comm_update(void)
{
    // Si querés después podemos agregar lógica de timeout o reconexión acá
}

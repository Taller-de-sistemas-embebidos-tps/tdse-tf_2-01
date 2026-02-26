//=====[Libraries]=============================================================
#include "comm.h"
#include "main.h"
#include <stdbool.h>

/* Demo includes */
#include "logger.h"
#include "dwt.h"
#include "systick.h"

//=====[Declaration of private defines]========================================

#define DISPLAY_DEL_37US	37ul
#define DISPLAY_DEL_01US	01ul

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of private global variables]============
static hm10_t hm10;

//=====[Declarations (prototypes) of private functions]========================
// static void uart_send_string(const char *str);

// static void uart_read_line(char *buffer, uint16_t max_len);

//=====[Implementations of public functions]===================================

void hm10_init(UART_HandleTypeDef *huart) {
    hm10.huart = huart;

    // Test básico AT (opcional)
    hm10_send_string("AT\r\n");
    HAL_Delay(100);

    hm10_send_string("AT+RESET\r\n");
    HAL_Delay(200);

    hm10_send_string("AT+NAMEActuator\r\n");
    HAL_Delay(100);

    hm10_send_string("AT+ROLE0\r\n");
    HAL_Delay(100);
}

HAL_StatusTypeDef hm10_send_data(uint8_t *data, uint16_t len) {
    return HAL_UART_Transmit(hm10.huart, data, len, 100);
}

HAL_StatusTypeDef hm10_send_string(const char *str) {
    return HAL_UART_Transmit(hm10.huart, (uint8_t *)str, strlen(str), 100);
}

/*
uint8_t hm10_available() {
    return ((hm10.uart)->SR & USART_SR_RXNE);
}

char hm10_read_byte() {
    while (!((hm10.uart)->SR & USART_SR_RXNE));
    return (hm10.uart)->DR;
}
*/

//=====[Implementations of private functions]==================================

/*
static void uart_send_string(const char *str) {
    while (*str) {
        while (!((hm10.uart)->SR & USART_SR_TXE));
        (hm10.uart)->DR = *str++;
    }
}

static void uart_read_line(char *buffer, uint16_t max_len) {
    uint16_t i = 0;
    char c;

    while (i < max_len - 1)    {
        while (!((hm10.uart)->SR & USART_SR_RXNE));
        c = (hm10.uart)->DR;

        buffer[i++] = c;

        if (c == '\n')
            break;
    }

    buffer[i] = '\0';
}
*/

/********************** end of file ******************************************/


//=====[#include guards - begin]===============================================

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <stdint.h>
#include "main.h"

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

typedef struct {
    UART_HandleTypeDef *huart;
} hm10_t;

//=====[Declarations (prototypes) of public functions]=========================

void hm10_init(UART_HandleTypeDef *huart);

HAL_StatusTypeDef hm10_send_data(uint8_t *data, uint16_t len);

HAL_StatusTypeDef hm10_send_string(const char *str);

HAL_StatusTypeDef hm10_receive_byte(uint8_t *byte, uint32_t timeout);

HAL_StatusTypeDef hm10_receive_buffer(uint8_t *buffer, uint16_t len, uint32_t timeout);

extern UART_HandleTypeDef huart2;

//=====[#include guards - end]=================================================

#endif // _BLUETOOTH_H_

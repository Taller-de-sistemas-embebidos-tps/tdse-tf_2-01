#ifndef MAX30102_H
#define MAX30102_H

#include "stm32f1xx_hal.h"

/* Dirección I2C (7 bits << 1 para HAL) */
#define MAX30102_I2C_ADDR   (0x57 << 1)

/* Registros */
#define REG_FIFO_DATA   0x07
#define REG_MODE_CONFIG 0x09
#define REG_SPO2_CONFIG 0x0A
#define REG_LED1_PA     0x0C
#define REG_LED2_PA     0x0D
#define REG_FIFO_CONFIG 0x08

typedef struct {
    I2C_HandleTypeDef *hi2c;
} max30102_t;

void MAX30102_Init(max30102_t *dev, I2C_HandleTypeDef *hi2c);
void MAX30102_ReadIR(max30102_t *dev, uint32_t *ir);

#endif

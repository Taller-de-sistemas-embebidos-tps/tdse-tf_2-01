#ifndef MAX30102_H
#define MAX30102_H

#include "stm32f1xx_hal.h"

/* Dirección I2C (7 bits << 1 para HAL) */
#define MAX30102_I2C_ADDR   (0x57 << 1)

/* Registros */
#define REG_INTR_STATUS_1   0x00
#define REG_INTR_STATUS_2   0x01
#define REG_FIFO_WR_PTR     0x04
#define REG_OVF_COUNTER     0x05
#define REG_FIFO_RD_PTR     0x06
#define REG_FIFO_DATA       0x07
#define REG_FIFO_CONFIG     0x08
#define REG_MODE_CONFIG     0x09
#define REG_SPO2_CONFIG     0x0A
#define REG_LED1_PA         0x0C
#define REG_LED2_PA         0x0D

typedef struct {
    I2C_HandleTypeDef *hi2c;
} max30102_t;

/* Funciones */
void MAX30102_Init(max30102_t *dev, I2C_HandleTypeDef *hi2c);
void MAX30102_Read(max30102_t *dev, uint32_t *red, uint32_t *ir);
#endif

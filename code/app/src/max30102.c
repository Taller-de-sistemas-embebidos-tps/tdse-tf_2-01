#include "max30102.h"

void MAX30102_Init(max30102_t *dev, I2C_HandleTypeDef *hi2c)
{
    dev->hi2c = hi2c;
    uint8_t data;

    /* Reset */
    data = 0x40;
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_MODE_CONFIG, 1,
                      &data, 1, HAL_MAX_DELAY);
    HAL_Delay(100);

    /* FIFO */
    data = 0x0F;
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_FIFO_CONFIG, 1,
                      &data, 1, HAL_MAX_DELAY);

    /* SPO2 config: 100 Hz, PW 411 us */
    data = 0x27;
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_SPO2_CONFIG, 1,
                      &data, 1, HAL_MAX_DELAY);

    /* LED currents */
    data = 0x24;
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_LED1_PA, 1,
                      &data, 1, HAL_MAX_DELAY);

    data = 0x24;
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_LED2_PA, 1,
                      &data, 1, HAL_MAX_DELAY);

    /* Mode: SpO2 (RED + IR) */
    data = 0x03;
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_MODE_CONFIG, 1,
                      &data, 1, HAL_MAX_DELAY);
}

void MAX30102_ReadIR(max30102_t *dev, uint32_t *ir)
{
    uint8_t buffer[6];

    HAL_I2C_Mem_Read(dev->hi2c,
                     MAX30102_I2C_ADDR,
                     REG_FIFO_DATA,
                     1,
                     buffer,
                     6,
                     HAL_MAX_DELAY);

    *ir = ((uint32_t)(buffer[3] & 0x03) << 16) |
          ((uint32_t)buffer[4] << 8) |
          buffer[5];
}

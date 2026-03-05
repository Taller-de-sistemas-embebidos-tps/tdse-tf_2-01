#include "max30102.h"

max30102_t max30102 = {
    &hi2c1,
};

void MAX30102_Init(max30102_t *dev, I2C_HandleTypeDef *hi2c)
{
    dev->hi2c = hi2c;
    uint8_t data;
    uint8_t zero = 0x00;

    /* Reset */
    data = 0x40;
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_MODE_CONFIG, 1,
                      &data, 1, HAL_MAX_DELAY);
    HAL_Delay(100);

    /* Limpiar FIFO */
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_FIFO_WR_PTR, 1,
                      &zero, 1, HAL_MAX_DELAY);

    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_OVF_COUNTER, 1,
                      &zero, 1, HAL_MAX_DELAY);

    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_FIFO_RD_PTR, 1,
                      &zero, 1, HAL_MAX_DELAY);

    /* FIFO config */
    data = 0x0F;
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_FIFO_CONFIG, 1,
                      &data, 1, HAL_MAX_DELAY);

    /* SpO2 config: 100 Hz, 411us pulse, 18-bit */
    data = 0x27;
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_SPO2_CONFIG, 1,
                      &data, 1, HAL_MAX_DELAY);

    /* Corriente LED RED */
    data = 0x24;
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_LED1_PA, 1,
                      &data, 1, HAL_MAX_DELAY);

    /* Corriente LED IR */
    data = 0x24;
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_LED2_PA, 1,
                      &data, 1, HAL_MAX_DELAY);

    /* Modo SpO2 (RED + IR activos) */
    data = 0x03;
    HAL_I2C_Mem_Write(dev->hi2c, MAX30102_I2C_ADDR,
                      REG_MODE_CONFIG, 1,
                      &data, 1, HAL_MAX_DELAY);
}

uint8_t MAX30102_DataAvailable(max30102_t *dev)
{
    uint8_t wr_ptr, rd_ptr;

    HAL_I2C_Mem_Read(dev->hi2c,
                     MAX30102_I2C_ADDR,
                     REG_FIFO_WR_PTR,
                     1,
                     &wr_ptr,
                     1,
                     HAL_MAX_DELAY);

    HAL_I2C_Mem_Read(dev->hi2c,
                     MAX30102_I2C_ADDR,
                     REG_FIFO_RD_PTR,
                     1,
                     &rd_ptr,
                     1,
                     HAL_MAX_DELAY);

    return (wr_ptr != rd_ptr);
}

void MAX30102_Read(max30102_t *dev, uint32_t *red, uint32_t *ir)
{
    uint8_t buffer[6];

    HAL_I2C_Mem_Read(dev->hi2c,
                     MAX30102_I2C_ADDR,
                     REG_FIFO_DATA,
                     1,
                     buffer,
                     6,
                     HAL_MAX_DELAY);

    *red = ((uint32_t)(buffer[0] & 0x03) << 16) |
           ((uint32_t)buffer[1] << 8) |
           buffer[2];

    *ir  = ((uint32_t)(buffer[3] & 0x03) << 16) |
           ((uint32_t)buffer[4] << 8) |
           buffer[5];
}

#include "memory.h"
#include "stm32f1xx_hal.h"  // <--- ¡Librería correcta para tu F103!
#include <string.h>         // <--- Necesario para el memcpy de memory_read

void memory_write(uint32_t address, void *data, uint32_t size)
{
    // 1. Desbloqueamos la memoria Flash para poder escribir
    HAL_FLASH_Unlock();

    // 2. Configuramos el borrado por "Página" (Específico del STM32F1)
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;

    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = address; // Borra la página de nuestra dirección
    EraseInitStruct.NbPages     = 1;       // Solo borramos 1 página

    // 3. Ejecutamos el borrado
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
        // Si hay error al borrar, volvemos a bloquear y salimos
        HAL_FLASH_Lock();
        return;
    }

    // 4. Escribimos los datos palabra por palabra (32 bits = 4 bytes)
    uint32_t *ptr = (uint32_t*)data;

    for (uint32_t i = 0; i < size; i += 4)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i, *ptr);
        ptr++;
    }

    // 5. Volvemos a bloquear la memoria por seguridad
    HAL_FLASH_Lock();
}

void memory_read(uint32_t address, void *data, uint32_t size)
{
    // La lectura en la Flash es directa como si fuera RAM
    memcpy(data, (void*)address, size);
}

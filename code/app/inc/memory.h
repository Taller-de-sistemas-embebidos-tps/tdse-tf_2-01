#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdint.h>

/* ==================================================================== */
/* API DE MEMORIA FLASH                                                 */
/* ==================================================================== */

/**
 * @brief Escribe un bloque de datos en la memoria Flash.
 * @param address Dirección física de la memoria (ej. 0x0801FC00)
 * @param data Puntero a los datos que quieres guardar
 * @param size Tamaño en bytes de los datos a guardar
 */
void memory_write(uint32_t address, void *data, uint32_t size);

/**
 * @brief Lee un bloque de datos desde la memoria Flash.
 * @param address Dirección física de la memoria de donde leer
 * @param data Puntero al lugar en RAM donde se volcarán los datos
 * @param size Tamaño en bytes a leer
 */
void memory_read(uint32_t address, void *data, uint32_t size);

#endif /* MEMORY_H_ */

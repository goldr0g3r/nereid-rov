/**
 * @file    FreeRTOSConfig.h
 * @brief   FreeRTOS configuration for the Linux host port (POSIX simulator).
 */
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "FreeRTOSConfig_Common.h"

/* POSIX port: tasks are pthreads with their own stacks; the FreeRTOS stack only
 * holds the port's thread bookkeeping, so overflow checking is meaningless. */
#define configMINIMAL_STACK_SIZE       ((unsigned short)256)
#define configCHECK_FOR_STACK_OVERFLOW 0

#endif /* FREERTOS_CONFIG_H */

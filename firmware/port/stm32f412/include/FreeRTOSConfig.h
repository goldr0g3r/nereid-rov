/**
 * @file    FreeRTOSConfig.h
 * @brief   FreeRTOS configuration for STM32F412 (Cortex-M4F, GCC_ARM_CM4F port).
 *
 * Requires the CubeMX settings in cubemx/README.md: HAL time base on TIM6 and no
 * generated SVC/PendSV/SysTick handlers (FreeRTOS provides them below).
 */
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "FreeRTOSConfig_Common.h"

#if !defined(__ASSEMBLER__)
#include <stdint.h>
extern uint32_t SystemCoreClock;
#endif

#define configCPU_CLOCK_HZ             (SystemCoreClock)
#define configMINIMAL_STACK_SIZE       ((unsigned short)128)
#define configCHECK_FOR_STACK_OVERFLOW 2

/* Cortex-M interrupt priorities: 4 priority bits on STM32F4. ISRs that call
 * FreeRTOS "FromISR" APIs must have a numeric priority >= MAX_SYSCALL (5). */
#define configPRIO_BITS                              4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY      15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY      (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* Map the port's exception handlers onto the CMSIS vector names. */
#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */

/**
 * @file    Mcu.h
 * @brief   MCAL: microcontroller control (reset reason, reset).
 *
 * Clock tree and pin multiplexing are configured by the port's generated start-up
 * code (STM32CubeMX on the STM32 port), which plays the role of AUTOSAR Mcu/Port init.
 */
#ifndef MCU_H
#define MCU_H

#include "Std_Types.h"

typedef enum
{
    MCU_RESET_POWER_ON = 0,
    MCU_RESET_PIN,
    MCU_RESET_WATCHDOG,
    MCU_RESET_SOFTWARE,
    MCU_RESET_BROWNOUT,
    MCU_RESET_UNKNOWN
} Mcu_ResetType;

void          Mcu_Init(void);
Mcu_ResetType Mcu_GetResetReason(void);
const char   *Mcu_GetResetReasonName(Mcu_ResetType reason);

/** Perform a software reset. Does not return. */
void Mcu_PerformReset(void);

#endif /* MCU_H */

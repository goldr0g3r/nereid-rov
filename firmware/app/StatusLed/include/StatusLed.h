/**
 * @file    StatusLed.h
 * @brief   StatusLed SWC: shows vehicle mode and fault class on two LEDs using
 *          table-driven blink patterns (16 steps of 100 ms = 1.6 s cycle).
 *          Pattern table: docs/software/state_machines.md#status-indication
 */
#ifndef STATUSLED_H
#define STATUSLED_H

#include "Rte_Types.h"

#define STATUSLED_PERIOD_MS 100u

void StatusLed_Init(void);
/** Runnable, STATUSLED_PERIOD_MS. */
void StatusLed_MainFunction(void);

/* Exposed for unit tests. */
uint16_t StatusLed_GetGreenPattern(Rte_VehicleModeType mode);
uint16_t StatusLed_GetRedPattern(Rte_VehicleModeType mode, Rte_FaultClassType cls);

#endif /* STATUSLED_H */

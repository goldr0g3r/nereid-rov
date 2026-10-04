/**
 * @file    Rte_StatusLed.h
 * @brief   RTE interface of the StatusLed SWC.
 */
#ifndef RTE_STATUSLED_H
#define RTE_STATUSLED_H

#include "Rte_Types.h"

Std_ReturnType Rte_Read_StatusLed_Mode(Rte_VehicleModeType *mode);
Std_ReturnType Rte_Read_StatusLed_FaultClass(Rte_FaultClassType *cls);
Std_ReturnType Rte_Write_StatusLed_GreenLed(bool on);
Std_ReturnType Rte_Write_StatusLed_RedLed(bool on);

#endif /* RTE_STATUSLED_H */

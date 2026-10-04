/**
 * @file    Rte_BenchPanel.h
 * @brief   RTE interface of the BenchPanel SWC (user button -> operator requests).
 */
#ifndef RTE_BENCHPANEL_H
#define RTE_BENCHPANEL_H

#include "Rte_Types.h"

Std_ReturnType Rte_Read_BenchPanel_UserButton(bool *pressed);
Std_ReturnType Rte_Read_BenchPanel_Mode(Rte_VehicleModeType *mode);
Std_ReturnType Rte_Write_BenchPanel_OperatorRequest(Rte_OperatorRequestType req); /* queued */

#endif /* RTE_BENCHPANEL_H */

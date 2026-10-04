/**
 * @file    Rte_Lights.h
 * @brief   RTE interface of the Lights SWC.
 */
#ifndef RTE_LIGHTS_H
#define RTE_LIGHTS_H

#include "Rte_Types.h"

Std_ReturnType Rte_Read_Lights_Request(Rte_LightsRequestType *req); /* queued */
Std_ReturnType Rte_Write_Lights_Output(bool on);

/** Sender side of the lights request queue (used by Com from M2, and by tests). */
Std_ReturnType Rte_Send_LightsRequest(Rte_LightsRequestType req);

#endif /* RTE_LIGHTS_H */

/**
 * @file    BswM.h
 * @brief   BSW mode manager: the single arbitration point between basic-software state
 *          (EcuM, Dem, later ComM/NvM) and the application's vehicle mode.
 *
 * Rules today:
 *  - fault class presented to the application = worst active Dem class;
 *  - self-test passed = EcuM in RUN and configuration valid;
 *  - an ECU reset/shutdown is only allowed while propulsion is disabled.
 * Future rules (communication loss -> failsafe request, NvM write windows...) go here,
 * not into individual SWCs.
 */
#ifndef BSWM_H
#define BSWM_H

#include "Dem.h"
#include "Std_Types.h"

void               BswM_Init(void);
void               BswM_MainFunction(void);
Dem_FaultClassType BswM_GetFaultClass(void);
bool               BswM_IsSelfTestPassed(void);
void               BswM_NotifyPropulsionEnabled(bool enabled);
bool               BswM_IsShutdownAllowed(void);

#endif /* BSWM_H */

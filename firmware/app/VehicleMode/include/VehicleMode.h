/**
 * @file    VehicleMode.h
 * @brief   VehicleMode SWC: the vehicle's top-level operating-mode state machine and
 *          propulsion-permission authority. Specification:
 *          docs/software/state_machines.md#vehiclemode
 */
#ifndef VEHICLEMODE_H
#define VEHICLEMODE_H

#include "Rte_Types.h"

typedef struct
{
    float manualAuthority;   /**< authority limit 0..1 in ARMED_MANUAL */
    float assistedAuthority; /**< authority limit 0..1 in ARMED_ASSISTED */
    float degradedAuthority; /**< authority limit 0..1 in ARMED_DEGRADED */
} VehicleMode_ConfigType;

void VehicleMode_Init(const VehicleMode_ConfigType *config);

/** Runnable, 10 ms: read inputs, process requests and faults, publish outputs. */
void VehicleMode_MainFunction(void);

/* Observers (diagnostics and unit tests). */
Rte_VehicleModeType  VehicleMode_GetMode(void);
Rte_ArmGuardMaskType VehicleMode_GetLastArmRejectMask(void);

#endif /* VEHICLEMODE_H */

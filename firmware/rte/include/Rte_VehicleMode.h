/**
 * @file    Rte_VehicleMode.h
 * @brief   RTE interface of the VehicleMode SWC (the only header it may use for I/O).
 */
#ifndef RTE_VEHICLEMODE_H
#define RTE_VEHICLEMODE_H

#include "Rte_Types.h"

/* Required ports */
Std_ReturnType Rte_Read_VehicleMode_OperatorRequest(Rte_OperatorRequestType *req); /* queued */
Std_ReturnType Rte_Read_VehicleMode_FaultClass(Rte_FaultClassType *cls);
Std_ReturnType Rte_Read_VehicleMode_ArmingInputs(Rte_ArmingInputsType *inputs);

/* Provided ports */
Std_ReturnType Rte_Write_VehicleMode_Mode(Rte_VehicleModeType mode);
Std_ReturnType Rte_Write_VehicleMode_PropulsionEnable(bool enable);
Std_ReturnType Rte_Write_VehicleMode_AuthorityLimit(float authority);
Std_ReturnType Rte_Write_VehicleMode_ArmRejectReason(Rte_ArmGuardMaskType mask);

/* Client/server: deliberate operator fault reset */
Std_ReturnType Rte_Call_VehicleMode_ClearFaults(void);

#endif /* RTE_VEHICLEMODE_H */

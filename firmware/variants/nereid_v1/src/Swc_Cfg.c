/**
 * @file    Swc_Cfg.c
 * @brief   Variant nereid_v1: SWC calibration values.
 */
#include "Swc_Cfg.h"

const VehicleMode_ConfigType VehicleMode_Config = {
    .manualAuthority   = 1.0f,
    .assistedAuthority = 1.0f,
    .degradedAuthority = 0.5f,
};

const BenchPanel_ConfigType BenchPanel_Config = {
    .periodMs    = 10u,
    .minPressMs  = 30u,
    .longPressMs = 1000u,
};

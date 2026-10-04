/**
 * @file    Dem_Cfg.c
 * @brief   Variant nereid_v1: fault classification (keep in sync with
 *          docs/safety/safety_case.md section 4, fault response matrix).
 */
#include "Dem.h"

const Dem_EventCfgType Dem_EventCfg[DEM_EVENT_COUNT] = {
    /*                              name                fault class         fail pass latched */
    [DEM_EVENT_DEV_ERROR]        = {"DEV_ERROR", DEM_CLASS_ADVISORY, 1, -1, true},
    [DEM_EVENT_TASK_OVERRUN]     = {"TASK_OVERRUN", DEM_CLASS_ADVISORY, 1, -1, true},
    [DEM_EVENT_WDGM_SUPERVISION] = {"WDGM_SUPERVISION", DEM_CLASS_FAILSAFE, 1, -1, true},
    [DEM_EVENT_WATCHDOG_RESET]   = {"WATCHDOG_RESET", DEM_CLASS_ADVISORY, 1, -1, true},
    [DEM_EVENT_CONFIG_INVALID]   = {"CONFIG_INVALID", DEM_CLASS_FAILSAFE, 1, -1, false},
};

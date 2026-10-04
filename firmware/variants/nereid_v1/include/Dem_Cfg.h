/**
 * @file    Dem_Cfg.h
 * @brief   Variant nereid_v1: diagnostic event identifiers. Classes, debounce and latching
 *          are in src/Dem_Cfg.c and mirrored in docs/safety/safety_case.md (fault matrix).
 */
#ifndef DEM_CFG_H
#define DEM_CFG_H

#include <stdint.h>

typedef uint8_t Dem_EventIdType;

enum
{
    DEM_EVENT_DEV_ERROR = 0,    /* Det reported a programming error        */
    DEM_EVENT_TASK_OVERRUN,     /* a task missed its period deadline        */
    DEM_EVENT_WDGM_SUPERVISION, /* alive supervision failed                 */
    DEM_EVENT_WATCHDOG_RESET,   /* last reset was caused by the watchdog    */
    DEM_EVENT_CONFIG_INVALID,   /* configuration/calibration check failed   */
    DEM_EVENT_COUNT
};

#endif /* DEM_CFG_H */

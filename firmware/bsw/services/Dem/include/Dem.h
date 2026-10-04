/**
 * @file    Dem.h
 * @brief   Diagnostic Event Manager: debounced, classified, optionally latched faults.
 *
 * Every monitored condition (sensor stale, leak, watchdog supervision, task overrun...)
 * is a Dem event. Monitors report PASSED/FAILED (immediate) or PREPASSED/PREFAILED
 * (counter debounce). Each event has a fault class taken from the safety case fault
 * response matrix (docs/safety/safety_case.md section 4):
 *   ADVISORY  -> inform operator only
 *   DEGRADED  -> reduced authority, assisted modes disabled
 *   FAILSAFE  -> propulsion neutral, operator reset required
 *   EMERGENCY -> emergency stop, latched
 * Dem_GetWorstActiveClass() is the single input the mode logic needs.
 */
#ifndef DEM_H
#define DEM_H

#include "Dem_Cfg.h" /* variant: Dem_EventIdType values DEM_EVENT_*, DEM_EVENT_COUNT */
#include "Std_Types.h"

typedef enum
{
    DEM_EVENT_STATUS_PASSED = 0,
    DEM_EVENT_STATUS_FAILED,
    DEM_EVENT_STATUS_PREPASSED,
    DEM_EVENT_STATUS_PREFAILED
} Dem_EventStatusType;

typedef enum
{
    DEM_CLASS_NONE = 0,
    DEM_CLASS_ADVISORY,
    DEM_CLASS_DEGRADED,
    DEM_CLASS_FAILSAFE,
    DEM_CLASS_EMERGENCY
} Dem_FaultClassType;

typedef struct
{
    const char        *name;
    Dem_FaultClassType faultClass;
    int8_t             failThreshold; /**< debounce counter value (> 0) that qualifies FAILED */
    int8_t             passThreshold; /**< debounce counter value (< 0) that qualifies PASSED */
    bool               latched;       /**< stays failed until Dem_ClearAllEvents() */
} Dem_EventCfgType;

extern const Dem_EventCfgType Dem_EventCfg[DEM_EVENT_COUNT];

void               Dem_Init(void);
Std_ReturnType     Dem_SetEventStatus(Dem_EventIdType id, Dem_EventStatusType status);
Std_ReturnType     Dem_GetEventFailed(Dem_EventIdType id, bool *failed);
uint16_t           Dem_GetOccurrenceCount(Dem_EventIdType id);
Dem_FaultClassType Dem_GetWorstActiveClass(void);
/**
 * Deliberate operator reset: un-latches every failed event whose monitor no longer
 * reports a failure. Events that are still failing stay failed.
 */
void Dem_ClearAllEvents(void);

#endif /* DEM_H */

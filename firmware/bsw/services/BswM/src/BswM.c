/**
 * @file    BswM.c
 * @brief   BSW mode manager.
 */
#include "BswM.h"

#include "EcuM.h"
#include "Log.h"

static volatile bool      BswM_PropulsionEnabled;
static Dem_FaultClassType BswM_LastClass;

void BswM_Init(void)
{
    BswM_PropulsionEnabled = false;
    BswM_LastClass         = DEM_CLASS_NONE;
}

void BswM_MainFunction(void)
{
    static const char *const names[] = {"NONE", "ADVISORY", "DEGRADED", "FAILSAFE", "EMERGENCY"};
    Dem_FaultClassType       cls     = Dem_GetWorstActiveClass();

    if (cls != BswM_LastClass)
    {
        LOG_INFO("BswM", "fault class %s -> %s", names[BswM_LastClass], names[cls]);
        BswM_LastClass = cls;
    }
}

Dem_FaultClassType BswM_GetFaultClass(void)
{
    return Dem_GetWorstActiveClass();
}

bool BswM_IsSelfTestPassed(void)
{
    bool configInvalid = true;

    (void)Dem_GetEventFailed(DEM_EVENT_CONFIG_INVALID, &configInvalid);
    return (EcuM_GetState() == ECUM_STATE_RUN) && !configInvalid;
}

void BswM_NotifyPropulsionEnabled(bool enabled)
{
    BswM_PropulsionEnabled = enabled;
}

bool BswM_IsShutdownAllowed(void)
{
    return !BswM_PropulsionEnabled;
}

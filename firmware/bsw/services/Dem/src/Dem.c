/**
 * @file    Dem.c
 * @brief   Diagnostic Event Manager.
 */
#include "Dem.h"

#include "Log.h"
#include "Os.h"

typedef struct
{
    int8_t   counter;         /**< debounce counter */
    bool     failed;          /**< qualified status */
    bool     conditionActive; /**< last report was a failure */
    uint16_t occurrences;     /**< passed -> failed transitions */
} Dem_EventStateType;

static Dem_EventStateType Dem_State[DEM_EVENT_COUNT];

void Dem_Init(void)
{
    uint32_t i;

    for (i = 0u; i < (uint32_t)DEM_EVENT_COUNT; i++)
    {
        Dem_State[i].counter         = 0;
        Dem_State[i].failed          = false;
        Dem_State[i].conditionActive = false;
        Dem_State[i].occurrences     = 0u;
    }
}

Std_ReturnType Dem_SetEventStatus(Dem_EventIdType id, Dem_EventStatusType status)
{
    const Dem_EventCfgType *cfg;
    Dem_EventStateType     *st;
    bool                    wasFailed;
    bool                    isFailed;

    if ((uint32_t)id >= (uint32_t)DEM_EVENT_COUNT)
    {
        return E_NOT_OK;
    }
    cfg = &Dem_EventCfg[id];
    st  = &Dem_State[id];

    Os_EnterCritical();
    wasFailed = st->failed;
    switch (status)
    {
    case DEM_EVENT_STATUS_FAILED:
        st->counter         = cfg->failThreshold;
        st->conditionActive = true;
        break;
    case DEM_EVENT_STATUS_PASSED:
        st->counter         = cfg->passThreshold;
        st->conditionActive = false;
        break;
    case DEM_EVENT_STATUS_PREFAILED:
        if (st->counter < 0)
        {
            st->counter = 0; /* jump-down: restart qualification from neutral */
        }
        if (st->counter < cfg->failThreshold)
        {
            st->counter++;
        }
        st->conditionActive = true;
        break;
    case DEM_EVENT_STATUS_PREPASSED:
        if (st->counter > 0)
        {
            st->counter = 0;
        }
        if (st->counter > cfg->passThreshold)
        {
            st->counter--;
        }
        st->conditionActive = false;
        break;
    default:
        Os_ExitCritical();
        return E_NOT_OK;
    }

    if (st->counter >= cfg->failThreshold)
    {
        st->failed = true;
    }
    else if ((st->counter <= cfg->passThreshold) && !cfg->latched)
    {
        st->failed = false;
    }
    else
    {
        /* keep qualified status while debouncing */
    }

    isFailed = st->failed;
    if (isFailed && !wasFailed && (st->occurrences < 0xFFFFu))
    {
        st->occurrences++;
    }
    Os_ExitCritical();

    if (isFailed != wasFailed)
    {
        if (isFailed)
        {
            LOG_WARN("Dem", "%s FAILED", cfg->name);
        }
        else
        {
            LOG_INFO("Dem", "%s passed", cfg->name);
        }
    }
    return E_OK;
}

Std_ReturnType Dem_GetEventFailed(Dem_EventIdType id, bool *failed)
{
    if (((uint32_t)id >= (uint32_t)DEM_EVENT_COUNT) || (failed == NULL))
    {
        return E_NOT_OK;
    }
    *failed = Dem_State[id].failed;
    return E_OK;
}

uint16_t Dem_GetOccurrenceCount(Dem_EventIdType id)
{
    return ((uint32_t)id < (uint32_t)DEM_EVENT_COUNT) ? Dem_State[id].occurrences : 0u;
}

Dem_FaultClassType Dem_GetWorstActiveClass(void)
{
    Dem_FaultClassType worst = DEM_CLASS_NONE;
    uint32_t           i;

    for (i = 0u; i < (uint32_t)DEM_EVENT_COUNT; i++)
    {
        if (Dem_State[i].failed && (Dem_EventCfg[i].faultClass > worst))
        {
            worst = Dem_EventCfg[i].faultClass;
        }
    }
    return worst;
}

void Dem_ClearAllEvents(void)
{
    uint32_t i;

    Os_EnterCritical();
    for (i = 0u; i < (uint32_t)DEM_EVENT_COUNT; i++)
    {
        if (Dem_State[i].failed && !Dem_State[i].conditionActive)
        {
            Dem_State[i].failed  = false;
            Dem_State[i].counter = 0;
        }
    }
    Os_ExitCritical();
    LOG_INFO("Dem", "inactive faults cleared by operator");
}

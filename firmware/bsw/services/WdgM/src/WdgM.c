/**
 * @file    WdgM.c
 * @brief   Watchdog manager (alive supervision).
 */
#include "WdgM.h"

#include "Dem.h"
#include "Log.h"
#include "Os.h"
#include "Wdg.h"

#define WDGM_MAX_ENTITIES 8u

static const WdgM_ConfigType *WdgM_Cfg;
static volatile uint16_t      WdgM_AliveCount[WDGM_MAX_ENTITIES];
static WdgM_GlobalStatusType  WdgM_Status;
static uint8_t                WdgM_FailedCycles;
static bool                   WdgM_FirstCycle;

void WdgM_Init(const WdgM_ConfigType *config)
{
    uint32_t i;

    WdgM_Cfg          = config;
    WdgM_Status       = WDGM_GLOBAL_STATUS_OK;
    WdgM_FailedCycles = 0u;
    WdgM_FirstCycle   = true;
    for (i = 0u; i < WDGM_MAX_ENTITIES; i++)
    {
        WdgM_AliveCount[i] = 0u;
    }
    if ((config == NULL) || (config->entityCount > WDGM_MAX_ENTITIES))
    {
        WdgM_Status = WDGM_GLOBAL_STATUS_EXPIRED;
        return;
    }
    Wdg_Init(config->wdgTimeoutMs);
}

void WdgM_CheckpointReached(WdgM_SupervisedEntityIdType se)
{
    if ((WdgM_Cfg != NULL) && ((uint32_t)se < WdgM_Cfg->entityCount))
    {
        Os_EnterCritical();
        if (WdgM_AliveCount[se] < 0xFFFFu)
        {
            WdgM_AliveCount[se]++;
        }
        Os_ExitCritical();
    }
}

void WdgM_MainFunction(void)
{
    uint16_t counts[WDGM_MAX_ENTITIES];
    bool     allOk = true;
    uint32_t i;

    if ((WdgM_Cfg == NULL) || (WdgM_Status == WDGM_GLOBAL_STATUS_EXPIRED))
    {
        return; /* never trigger again: let the hardware watchdog reset the MCU */
    }

    Os_EnterCritical();
    for (i = 0u; i < WdgM_Cfg->entityCount; i++)
    {
        counts[i]          = WdgM_AliveCount[i];
        WdgM_AliveCount[i] = 0u;
    }
    Os_ExitCritical();

    if (WdgM_FirstCycle)
    {
        /* Tasks start with different phase offsets; the first window is partial. */
        WdgM_FirstCycle = false;
        Wdg_Trigger();
        return;
    }

    for (i = 0u; i < WdgM_Cfg->entityCount; i++)
    {
        const WdgM_SupervisedEntityCfgType *se = &WdgM_Cfg->entities[i];
        if ((counts[i] < se->minAlive) || (counts[i] > se->maxAlive))
        {
            allOk = false;
            LOG_WARN("WdgM", "SE %s alive=%u expected %u..%u", se->name, (unsigned)counts[i],
                     (unsigned)se->minAlive, (unsigned)se->maxAlive);
        }
    }

    if (allOk)
    {
        WdgM_FailedCycles = 0u;
        WdgM_Status       = WDGM_GLOBAL_STATUS_OK;
        Wdg_Trigger();
        return;
    }

    WdgM_FailedCycles++;
    (void)Dem_SetEventStatus(DEM_EVENT_WDGM_SUPERVISION, DEM_EVENT_STATUS_FAILED);
    if (WdgM_FailedCycles > WdgM_Cfg->failedCycleTolerance)
    {
        WdgM_Status = WDGM_GLOBAL_STATUS_EXPIRED;
        LOG_ERROR("WdgM", "supervision expired, watchdog reset follows");
        Log_Flush();
    }
    else
    {
        WdgM_Status = WDGM_GLOBAL_STATUS_FAILED;
        Wdg_Trigger();
    }
}

WdgM_GlobalStatusType WdgM_GetGlobalStatus(void)
{
    return WdgM_Status;
}

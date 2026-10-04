/**
 * @file    SchM.c
 * @brief   Schedule manager: task creation, execution-time and overrun monitoring.
 */
#include "SchM.h"

#include "Dem.h"
#include "Gpt.h"
#include "Log.h"
#include "Os.h"

static SchM_TaskStatsType SchM_Stats[OS_MAX_TASKS];

static void SchM_TaskBody(void *arg)
{
    uint32_t                index = (uint32_t)(uintptr_t)arg;
    SchM_TaskStatsType     *st    = &SchM_Stats[index];
    uint32_t                start = Gpt_GetTimeUs();
    uint32_t                exec;

    SchM_TaskCfg[index].body();

    exec            = Gpt_ElapsedUs(start, Gpt_GetTimeUs());
    st->lastExecUs  = exec;
    st->activations++;
    if (exec > st->maxExecUs)
    {
        st->maxExecUs = exec;
    }
}

static void SchM_TaskOverrun(void *arg)
{
    uint32_t index = (uint32_t)(uintptr_t)arg;

    SchM_Stats[index].overruns++;
    (void)Dem_SetEventStatus(DEM_EVENT_TASK_OVERRUN, DEM_EVENT_STATUS_FAILED);
    LOG_WARN("SchM", "task %s overran its %lu ms period", SchM_TaskCfg[index].name,
             (unsigned long)SchM_TaskCfg[index].periodMs);
}

Std_ReturnType SchM_Init(void)
{
    uint32_t i;

    if (SchM_TaskCount > OS_MAX_TASKS)
    {
        return E_NOT_OK;
    }
    for (i = 0u; i < (uint32_t)SchM_TaskCount; i++)
    {
        const Os_PeriodicTaskType task = {
            .name       = SchM_TaskCfg[i].name,
            .periodMs   = SchM_TaskCfg[i].periodMs,
            .priority   = SchM_TaskCfg[i].priority,
            .stackWords = SchM_TaskCfg[i].stackWords,
            .body       = SchM_TaskBody,
            .onOverrun  = SchM_TaskOverrun,
            .arg        = (void *)(uintptr_t)i,
        };
        if (Os_CreatePeriodicTask(&task) != E_OK)
        {
            LOG_ERROR("SchM", "cannot create task %s", SchM_TaskCfg[i].name);
            return E_NOT_OK;
        }
    }
    return E_OK;
}

Std_ReturnType SchM_GetTaskStats(uint8_t taskIndex, SchM_TaskStatsType *stats)
{
    if ((taskIndex >= SchM_TaskCount) || (stats == NULL))
    {
        return E_NOT_OK;
    }
    Os_EnterCritical();
    *stats = SchM_Stats[taskIndex];
    Os_ExitCritical();
    return E_OK;
}

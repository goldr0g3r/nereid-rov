/**
 * @file    WdgM.h
 * @brief   Watchdog manager: alive supervision of supervised entities (SE).
 *
 * Each SE (e.g. the 10 ms control task) reports WdgM_CheckpointReached() every time it
 * runs. Once per supervision cycle WdgM_MainFunction() checks that every SE reported
 * between minAlive and maxAlive times. Only if all SEs are healthy (or the failure is
 * still within the tolerance) is the hardware watchdog triggered; otherwise triggering
 * stops and the independent watchdog resets the MCU. A hung or starved task therefore
 * causes a reset even though some other task is still running.
 */
#ifndef WDGM_H
#define WDGM_H

#include "Std_Types.h"
#include "WdgM_Cfg.h" /* variant: WdgM_SupervisedEntityIdType values WDGM_SE_* */

typedef struct
{
    const char *name;
    uint16_t    minAlive; /**< per supervision cycle */
    uint16_t    maxAlive; /**< per supervision cycle */
} WdgM_SupervisedEntityCfgType;

typedef struct
{
    uint32_t                            wdgTimeoutMs;
    uint8_t                             failedCycleTolerance;
    const WdgM_SupervisedEntityCfgType *entities;
    uint8_t                             entityCount;
} WdgM_ConfigType;

typedef enum
{
    WDGM_GLOBAL_STATUS_OK = 0,
    WDGM_GLOBAL_STATUS_FAILED,  /**< failing, still within tolerance, still triggering */
    WDGM_GLOBAL_STATUS_EXPIRED  /**< triggering stopped, reset imminent */
} WdgM_GlobalStatusType;

extern const WdgM_ConfigType WdgM_Config;

void                  WdgM_Init(const WdgM_ConfigType *config);
void                  WdgM_CheckpointReached(WdgM_SupervisedEntityIdType se);
void                  WdgM_MainFunction(void);
WdgM_GlobalStatusType WdgM_GetGlobalStatus(void);

#endif /* WDGM_H */

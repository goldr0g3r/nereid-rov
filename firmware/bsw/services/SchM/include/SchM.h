/**
 * @file    SchM.h
 * @brief   Schedule manager: creates the variant's task set and measures timing.
 *
 * The task table (period, priority, stack, body) is variant configuration
 * (variants/<rov>/src/SchM_Cfg.c); the task bodies are RTE functions that call
 * BSW main functions and SWC runnables in a fixed order.
 */
#ifndef SCHM_H
#define SCHM_H

#include "Std_Types.h"

typedef struct
{
    const char *name;
    uint32_t    periodMs;
    uint8_t     priority;
    uint32_t    stackWords;
    void (*body)(void);
} SchM_TaskCfgType;

typedef struct
{
    uint32_t activations;
    uint32_t overruns;
    uint32_t lastExecUs;
    uint32_t maxExecUs;
} SchM_TaskStatsType;

extern const SchM_TaskCfgType SchM_TaskCfg[];
extern const uint8_t          SchM_TaskCount;

Std_ReturnType SchM_Init(void);
Std_ReturnType SchM_GetTaskStats(uint8_t taskIndex, SchM_TaskStatsType *stats);

#endif /* SCHM_H */

/**
 * @file    WdgM_Cfg.c
 * @brief   Variant nereid_v1: alive supervision. Supervision cycle = 100 ms (BSW task).
 */
#include "WdgM.h"

static const WdgM_SupervisedEntityCfgType WdgM_Entities[WDGM_SE_COUNT] = {
    /* 10 ms task -> 10 checkpoints per 100 ms cycle, +/-2 for phase jitter */
    [WDGM_SE_CONTROL_10MS] = {.name = "Control10ms", .minAlive = 8u, .maxAlive = 12u},
};

const WdgM_ConfigType WdgM_Config = {
    .wdgTimeoutMs         = 500u,
    .failedCycleTolerance = 1u,
    .entities             = WdgM_Entities,
    .entityCount          = WDGM_SE_COUNT,
};

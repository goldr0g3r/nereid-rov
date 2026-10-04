/**
 * @file    SchM_Cfg.c
 * @brief   Variant nereid_v1: task set. Budgets: docs/software/firmware_architecture.md#tasks
 */
#include "Rte_Main.h"
#include "SchM.h"

const SchM_TaskCfgType SchM_TaskCfg[] = {
    /* name          period  prio  stack(words) body */
    {"Control10ms", 10u, 5u, 768u, Rte_Task_Control10ms},
    {"Bsw100ms", 100u, 2u, 768u, Rte_Task_Bsw100ms},
};

const uint8_t SchM_TaskCount = (uint8_t)STD_ARRAY_SIZE(SchM_TaskCfg);

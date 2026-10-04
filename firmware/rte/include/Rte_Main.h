/**
 * @file    Rte_Main.h
 * @brief   RTE life cycle and task bodies (used by EcuM/SchM configuration only).
 */
#ifndef RTE_MAIN_H
#define RTE_MAIN_H

#include "Std_Types.h"

/** Initialise RTE buffers and call every SWC's init runnable. */
Std_ReturnType Rte_Start(void);

/* Task bodies: fixed runnable order per task (see docs/software/firmware_architecture.md). */
void Rte_Task_Control10ms(void);
void Rte_Task_Bsw100ms(void);

#endif /* RTE_MAIN_H */

/**
 * @file    EcuM.h
 * @brief   ECU state manager: start-up sequence, RUN, controlled shutdown/reset and
 *          fatal-error handling. Implemented as a state machine on lib/Fsm:
 *
 *            STARTUP --init done--> RUN --reset request & BswM permits--> SHUTDOWN
 *
 * The concrete driver/service init lists are variant configuration
 * (variants/<rov>/src/EcuM_Cfg.c), mirroring AUTOSAR's EcuM init lists.
 */
#ifndef ECUM_H
#define ECUM_H

#include "Std_Types.h"

typedef enum
{
    ECUM_STATE_STARTUP = 0,
    ECUM_STATE_RUN,
    ECUM_STATE_SHUTDOWN
} EcuM_StateType;

typedef enum
{
    ECUM_ERROR_OS_ASSERT = 0,
    ECUM_ERROR_STACK_OVERFLOW,
    ECUM_ERROR_INIT_FAILED,
    ECUM_ERROR_HARD_FAULT
} EcuM_ErrorType;

/** Entry point called by the port's main() after clock and pin initialisation.
 *  Runs the init lists, creates tasks, enters RUN and starts the OS. Does not return. */
void EcuM_Init(void);

/** Cyclic: executes pending shutdown/reset requests (100 ms task). */
void EcuM_MainFunction(void);

/** Request a controlled reset. Executed once BswM reports shutdown is allowed. */
void           EcuM_RequestReset(void);
EcuM_StateType EcuM_GetState(void);
const char    *EcuM_GetStateName(EcuM_StateType state);

/** Fatal error: force outputs safe, log, then reset. Does not return. */
void EcuM_ErrorHook(EcuM_ErrorType error, const char *detail);

/* ---- Variant callouts (variants/<rov>/src/EcuM_Cfg.c) ---- */

/** Before the OS exists: MCAL drivers, Det, Log, Dem. */
void EcuM_Cfg_InitListZero(void);
/** ECU abstraction, services, RTE + SWC init, WdgM, task creation. */
Std_ReturnType EcuM_Cfg_InitListOne(void);
/** Human readable variant name + firmware version for the boot banner. */
const char *EcuM_Cfg_GetVariantName(void);

#endif /* ECUM_H */

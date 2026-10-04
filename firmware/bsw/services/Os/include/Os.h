/**
 * @file    Os.h
 * @brief   OS abstraction. The only module that includes FreeRTOS headers.
 *
 * Everything above this API is RTOS-agnostic, so replacing FreeRTOS (e.g. with Zephyr,
 * see docs/decisions/ADR-004-rtos.md) only touches Os.c. Tasks are statically
 * allocated, strictly periodic (AUTOSAR-OS style "basic tasks" executing runnables)
 * and created before Os_Start().
 */
#ifndef OS_H
#define OS_H

#include "Std_Types.h"

#define OS_MAX_TASKS 8u

typedef struct
{
    const char *name;
    uint32_t    periodMs;
    uint8_t     priority;   /**< 1 (lowest) .. OS_MAX_PRIORITY; higher preempts lower */
    uint32_t    stackWords; /**< stack size in StackType_t words */
    void (*body)(void *arg);      /**< executed once per period */
    void (*onOverrun)(void *arg); /**< called when a period deadline was missed (may be NULL) */
    void *arg;
} Os_PeriodicTaskType;

#define OS_MAX_PRIORITY 6u

/** Create a periodic task. Only valid before Os_Start(). */
Std_ReturnType Os_CreatePeriodicTask(const Os_PeriodicTaskType *task);

/** Start the scheduler. Does not return. */
void Os_Start(void);

bool     Os_IsRunning(void);
uint32_t Os_GetTimeMs(void);

/** Nestable critical section (task context). No-op before the scheduler starts. */
void Os_EnterCritical(void);
void Os_ExitCritical(void);

/** Called by the FreeRTOS configASSERT() hook. Does not return. */
void Os_AssertFailed(const char *file, int line);

#endif /* OS_H */

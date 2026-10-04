/**
 * @file    Os.c
 * @brief   OS abstraction implemented on FreeRTOS with static allocation only.
 */
#include "Os.h"

#include "EcuM.h"
#include "FreeRTOS.h"
#include "task.h"

#ifndef OS_STACK_POOL_WORDS
#define OS_STACK_POOL_WORDS 4096u
#endif

STD_STATIC_ASSERT(OS_MAX_PRIORITY < configMAX_PRIORITIES, "OS_MAX_PRIORITY exceeds configMAX_PRIORITIES");

typedef struct
{
    Os_PeriodicTaskType cfg;
    StaticTask_t        tcb;
} Os_TaskSlotType;

static Os_TaskSlotType Os_Tasks[OS_MAX_TASKS];
static uint32_t        Os_TaskCount;
static StackType_t     Os_StackPool[OS_STACK_POOL_WORDS];
static uint32_t        Os_StackUsed;

static void Os_PeriodicTaskEntry(void *param)
{
    const Os_TaskSlotType *slot     = (const Os_TaskSlotType *)param;
    const TickType_t       period   = pdMS_TO_TICKS(slot->cfg.periodMs);
    TickType_t             lastWake = xTaskGetTickCount();

    for (;;)
    {
        slot->cfg.body(slot->cfg.arg);

        /* xTaskDelayUntil() returns pdFALSE when the next release time has already
         * passed, i.e. the body overran its period. */
        if ((xTaskDelayUntil(&lastWake, period) == pdFALSE) && (slot->cfg.onOverrun != NULL))
        {
            slot->cfg.onOverrun(slot->cfg.arg);
            lastWake = xTaskGetTickCount(); /* re-synchronise instead of bursting */
        }
    }
}

Std_ReturnType Os_CreatePeriodicTask(const Os_PeriodicTaskType *task)
{
    Os_TaskSlotType *slot;
    uint32_t         words;
    TaskHandle_t     handle;

    if ((task == NULL) || (task->body == NULL) || (task->periodMs == 0u) || (task->priority == 0u) ||
        (task->priority > OS_MAX_PRIORITY) || (Os_TaskCount >= OS_MAX_TASKS) || Os_IsRunning())
    {
        return E_NOT_OK;
    }

    words = (task->stackWords < configMINIMAL_STACK_SIZE) ? configMINIMAL_STACK_SIZE : task->stackWords;
    if ((Os_StackUsed + words) > OS_STACK_POOL_WORDS)
    {
        return E_NOT_OK;
    }

    slot      = &Os_Tasks[Os_TaskCount];
    slot->cfg = *task;
    handle    = xTaskCreateStatic(Os_PeriodicTaskEntry, task->name, words, slot, (UBaseType_t)task->priority,
                                  &Os_StackPool[Os_StackUsed], &slot->tcb);
    if (handle == NULL)
    {
        return E_NOT_OK;
    }
    Os_StackUsed += words;
    Os_TaskCount++;
    return E_OK;
}

void Os_Start(void)
{
    vTaskStartScheduler();
    /* Only reached if the scheduler could not start. */
    Os_AssertFailed(__FILE__, __LINE__);
}

bool Os_IsRunning(void)
{
    return xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED;
}

uint32_t Os_GetTimeMs(void)
{
    return Os_IsRunning() ? (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS) : 0u;
}

void Os_EnterCritical(void)
{
    if (Os_IsRunning())
    {
        taskENTER_CRITICAL();
    }
}

void Os_ExitCritical(void)
{
    if (Os_IsRunning())
    {
        taskEXIT_CRITICAL();
    }
}

void Os_AssertFailed(const char *file, int line)
{
    STD_UNUSED(line);
    EcuM_ErrorHook(ECUM_ERROR_OS_ASSERT, file);
}

/* ---- FreeRTOS hooks ------------------------------------------------------- */

void vApplicationStackOverflowHook(TaskHandle_t task, char *taskName)
{
    STD_UNUSED(task);
    EcuM_ErrorHook(ECUM_ERROR_STACK_OVERFLOW, taskName);
}

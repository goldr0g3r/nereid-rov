/**
 * @file    FreeRTOSConfig_Common.h
 * @brief   FreeRTOS settings shared by every port. Each port's FreeRTOSConfig.h includes
 *          this file and adds CPU/interrupt specifics.
 *
 * Policy: static allocation only, 1 ms tick, no software timers, no dynamic heap.
 */
#ifndef FREERTOS_CONFIG_COMMON_H
#define FREERTOS_CONFIG_COMMON_H

#define configUSE_PREEMPTION                1
#define configUSE_TIME_SLICING              0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configTICK_RATE_HZ                  1000
#define configMAX_PRIORITIES                8
#define configMAX_TASK_NAME_LEN             16
#define configTICK_TYPE_WIDTH_IN_BITS       TICK_TYPE_WIDTH_32_BITS
#define configIDLE_SHOULD_YIELD             1
#define configUSE_IDLE_HOOK                 0
#define configUSE_TICK_HOOK                 0
#define configUSE_MUTEXES                   1
#define configUSE_RECURSIVE_MUTEXES         0
#define configUSE_COUNTING_SEMAPHORES       0
#define configUSE_TASK_NOTIFICATIONS        1
#define configQUEUE_REGISTRY_SIZE           0
#define configUSE_TIMERS                    0
#define configUSE_CO_ROUTINES               0
#define configUSE_TRACE_FACILITY            0
#define configGENERATE_RUN_TIME_STATS       0

#define configSUPPORT_STATIC_ALLOCATION     1
#define configSUPPORT_DYNAMIC_ALLOCATION    0
#define configKERNEL_PROVIDED_STATIC_MEMORY 1

#define INCLUDE_xTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_vTaskDelete                 0
#define INCLUDE_vTaskSuspend                0
#define INCLUDE_xTaskGetSchedulerState      1
#define INCLUDE_xTaskGetCurrentTaskHandle   1
#define INCLUDE_uxTaskGetStackHighWaterMark 1

void Os_AssertFailed(const char *file, int line);
#define configASSERT(x)                         \
    if ((x) == 0)                               \
    {                                           \
        Os_AssertFailed(__FILE__, __LINE__);    \
    }

#endif /* FREERTOS_CONFIG_COMMON_H */

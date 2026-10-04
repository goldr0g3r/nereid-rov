/**
 * @file    Log.h
 * @brief   Non-blocking text logger.
 *
 * Log_Write() formats into a ring buffer (task context only, never from ISRs) and
 * returns immediately; Log_MainFunction() drains the buffer to the debug UART from
 * the lowest-priority task. Lines that do not fit are dropped and counted, so logging
 * can never stall the control loop.
 */
#ifndef LOG_H
#define LOG_H

#include "Std_Types.h"

typedef enum
{
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} Log_LevelType;

void Log_Init(void);
void Log_Write(Log_LevelType level, const char *module, const char *fmt, ...) __attribute__((format(printf, 3, 4)));
/** Drain buffered text to the UART (bounded amount per call). */
void Log_MainFunction(void);
/** Drain everything (blocking). Used during shutdown only. */
void     Log_Flush(void);
uint32_t Log_GetDroppedCount(void);

#define LOG_DEBUG(mod, ...) Log_Write(LOG_LEVEL_DEBUG, (mod), __VA_ARGS__)
#define LOG_INFO(mod, ...)  Log_Write(LOG_LEVEL_INFO, (mod), __VA_ARGS__)
#define LOG_WARN(mod, ...)  Log_Write(LOG_LEVEL_WARN, (mod), __VA_ARGS__)
#define LOG_ERROR(mod, ...) Log_Write(LOG_LEVEL_ERROR, (mod), __VA_ARGS__)

#endif /* LOG_H */

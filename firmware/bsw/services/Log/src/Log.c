/**
 * @file    Log.c
 * @brief   Ring-buffered logger. Format: "[    12.345] I Module: text\r\n"
 */
#include "Log.h"

#include <stdarg.h>
#include <stdio.h>

#include "Os.h"
#include "RingBuf.h"
#include "Uart.h"

#define LOG_BUFFER_SIZE  2048u
#define LOG_LINE_MAX     128u
#define LOG_DRAIN_CHUNK  64u
#define LOG_DRAIN_BUDGET 512u /* bytes per Log_MainFunction call */

static uint8_t           Log_Storage[LOG_BUFFER_SIZE];
static RingBuf_Type      Log_Buffer;
static volatile uint32_t Log_Dropped;
static bool              Log_Initialized;

static const char Log_LevelChar[] = {'D', 'I', 'W', 'E'};

void Log_Init(void)
{
    (void)RingBuf_Init(&Log_Buffer, Log_Storage, LOG_BUFFER_SIZE);
    Log_Dropped     = 0u;
    Log_Initialized = true;
}

void Log_Write(Log_LevelType level, const char *module, const char *fmt, ...)
{
    char     line[LOG_LINE_MAX];
    uint32_t ms = Os_GetTimeMs();
    int      n;
    int      m;
    va_list  args;

    if (!Log_Initialized)
    {
        return;
    }

    n = snprintf(line, sizeof(line), "[%6lu.%03lu] %c %s: ", (unsigned long)(ms / 1000u),
                 (unsigned long)(ms % 1000u), Log_LevelChar[(uint32_t)level & 3u], module);
    if ((n < 0) || ((uint32_t)n >= sizeof(line)))
    {
        return;
    }

    va_start(args, fmt);
    m = vsnprintf(&line[n], sizeof(line) - (size_t)n, fmt, args);
    va_end(args);
    if (m < 0)
    {
        return;
    }
    n += m;
    if ((uint32_t)n > (sizeof(line) - 3u)) /* truncated: keep room for CRLF */
    {
        n = (int)(sizeof(line) - 3u);
    }
    line[n]     = '\r';
    line[n + 1] = '\n';
    n += 2;

    Os_EnterCritical();
    if (RingBuf_Free(&Log_Buffer) >= (uint32_t)n)
    {
        (void)RingBuf_Write(&Log_Buffer, (const uint8_t *)line, (uint32_t)n);
    }
    else
    {
        Log_Dropped++;
    }
    Os_ExitCritical();
}

static uint32_t Log_Drain(uint32_t budget)
{
    uint8_t  chunk[LOG_DRAIN_CHUNK];
    uint32_t total = 0u;

    while (total < budget)
    {
        uint32_t n = RingBuf_Read(&Log_Buffer, chunk, sizeof(chunk));
        if (n == 0u)
        {
            break;
        }
        (void)Uart_Write(UART_CH_DEBUG, chunk, n);
        total += n;
    }
    return total;
}

void Log_MainFunction(void)
{
    if (Log_Initialized)
    {
        (void)Log_Drain(LOG_DRAIN_BUDGET);
    }
}

void Log_Flush(void)
{
    if (Log_Initialized)
    {
        (void)Log_Drain(LOG_BUFFER_SIZE);
    }
}

uint32_t Log_GetDroppedCount(void)
{
    return Log_Dropped;
}

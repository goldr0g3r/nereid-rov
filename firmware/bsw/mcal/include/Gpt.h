/**
 * @file    Gpt.h
 * @brief   MCAL: free-running microsecond time base for timestamps and execution-time
 *          measurement. Wraps after ~71 minutes; always compare with unsigned subtraction.
 */
#ifndef GPT_H
#define GPT_H

#include "Std_Types.h"

void     Gpt_Init(void);
uint32_t Gpt_GetTimeUs(void);

/** Elapsed microseconds between two Gpt_GetTimeUs() samples (wrap-safe). */
static inline uint32_t Gpt_ElapsedUs(uint32_t start, uint32_t end)
{
    return end - start;
}

#endif /* GPT_H */

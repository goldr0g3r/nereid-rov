/**
 * @file    Gpt_Host.c
 * @brief   Host MCAL: Gpt from CLOCK_MONOTONIC.
 */
#include "Gpt.h"

#include <time.h>

static uint64_t Gpt_StartUs;

static uint64_t Gpt_NowUs(void)
{
    struct timespec ts;
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec * 1000000u) + ((uint64_t)ts.tv_nsec / 1000u);
}

void Gpt_Init(void)
{
    Gpt_StartUs = Gpt_NowUs();
}

uint32_t Gpt_GetTimeUs(void)
{
    return (uint32_t)(Gpt_NowUs() - Gpt_StartUs);
}

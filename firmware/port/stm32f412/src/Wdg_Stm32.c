/**
 * @file    Wdg_Stm32.c
 * @brief   STM32 MCAL: independent watchdog (IWDG, clocked by the ~32 kHz LSI).
 *
 * Note: LSI tolerance on STM32F4 is wide (17..47 kHz), so the effective timeout is
 * roughly 0.7x..1.9x the requested value. WdgM triggers every 100 ms; keep the timeout
 * >= 5x that period. In Debug builds the IWDG is frozen while the core is halted.
 */
#include "Wdg.h"

#include "stm32f4xx_hal.h"

#define WDG_KEY_RELOAD 0xAAAAu
#define WDG_KEY_ENABLE 0x5555u
#define WDG_KEY_START  0xCCCCu
#define WDG_PRESCALER_64 4u /* 32 kHz / 64 = 500 Hz -> 2 ms per count */

void Wdg_Init(uint32_t timeoutMs)
{
    uint32_t reload = timeoutMs / 2u;

    if (reload < 1u)
    {
        reload = 1u;
    }
    if (reload > 0x0FFFu)
    {
        reload = 0x0FFFu;
    }

#ifdef DEBUG
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_IWDG_STOP;
#endif
    IWDG->KR  = WDG_KEY_START;
    IWDG->KR  = WDG_KEY_ENABLE;
    IWDG->PR  = WDG_PRESCALER_64;
    IWDG->RLR = reload;
    while (IWDG->SR != 0u)
    {
        /* wait until PR/RLR are updated (a few LSI cycles) */
    }
    IWDG->KR = WDG_KEY_RELOAD;
}

void Wdg_Trigger(void)
{
    IWDG->KR = WDG_KEY_RELOAD;
}

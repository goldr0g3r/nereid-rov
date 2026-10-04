/**
 * @file    Gpt_Stm32.c
 * @brief   STM32 MCAL: Gpt on TIM2 (32-bit) running free at 1 MHz.
 *          TIM2 is reserved for this purpose (docs/hardware/ecu_resource_allocation.md).
 */
#include "Gpt.h"

#include "stm32f4xx_hal.h"

void Gpt_Init(void)
{
    uint32_t pclk1  = HAL_RCC_GetPCLK1Freq();
    /* Timer clock is PCLK1 x2 whenever the APB1 prescaler is not 1. */
    uint32_t timclk = ((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV1) ? pclk1 : (2u * pclk1);

    __HAL_RCC_TIM2_CLK_ENABLE();
    TIM2->CR1 = 0u;
    TIM2->PSC = (timclk / 1000000u) - 1u;
    TIM2->ARR = 0xFFFFFFFFu;
    TIM2->CNT = 0u;
    TIM2->EGR = TIM_EGR_UG; /* load the prescaler */
    TIM2->CR1 = TIM_CR1_CEN;
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_TIM2_STOP;
}

uint32_t Gpt_GetTimeUs(void)
{
    return TIM2->CNT;
}

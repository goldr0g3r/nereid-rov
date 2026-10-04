/**
 * @file    Mcu_Stm32.c
 * @brief   STM32 MCAL: Mcu (reset cause from RCC_CSR, software reset).
 *          Clocks are configured by CubeMX SystemClock_Config() before EcuM_Init().
 */
#include "Mcu.h"

#include "stm32f4xx_hal.h"

static Mcu_ResetType Mcu_Reason = MCU_RESET_UNKNOWN;

void Mcu_Init(void)
{
    uint32_t csr = RCC->CSR;

    /* Order matters: a power-on reset also sets the BOR and PIN flags. */
    if ((csr & (RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF)) != 0u)
    {
        Mcu_Reason = MCU_RESET_WATCHDOG;
    }
    else if ((csr & RCC_CSR_SFTRSTF) != 0u)
    {
        Mcu_Reason = MCU_RESET_SOFTWARE;
    }
    else if ((csr & RCC_CSR_PORRSTF) != 0u)
    {
        Mcu_Reason = MCU_RESET_POWER_ON;
    }
    else if ((csr & RCC_CSR_BORRSTF) != 0u)
    {
        Mcu_Reason = MCU_RESET_BROWNOUT;
    }
    else if ((csr & RCC_CSR_PINRSTF) != 0u)
    {
        Mcu_Reason = MCU_RESET_PIN;
    }
    else
    {
        Mcu_Reason = MCU_RESET_UNKNOWN;
    }
    RCC->CSR |= RCC_CSR_RMVF; /* clear flags for the next boot */
}

Mcu_ResetType Mcu_GetResetReason(void)
{
    return Mcu_Reason;
}

const char *Mcu_GetResetReasonName(Mcu_ResetType reason)
{
    static const char *const names[] = {"POWER_ON", "PIN", "WATCHDOG", "SOFTWARE", "BROWNOUT", "UNKNOWN"};
    return ((uint32_t)reason < STD_ARRAY_SIZE(names)) ? names[reason] : "UNKNOWN";
}

void Mcu_PerformReset(void)
{
    NVIC_SystemReset();
}

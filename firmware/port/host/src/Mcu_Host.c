/**
 * @file    Mcu_Host.c
 * @brief   Host MCAL: Mcu. A reset ends the simulation (exit code 0 = requested reset).
 */
#include "HostSim.h"
#include "Mcu.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void Mcu_Init(void)
{
}

Mcu_ResetType Mcu_GetResetReason(void)
{
    return MCU_RESET_POWER_ON;
}

const char *Mcu_GetResetReasonName(Mcu_ResetType reason)
{
    static const char *const names[] = {"POWER_ON", "PIN", "WATCHDOG", "SOFTWARE", "BROWNOUT", "UNKNOWN"};
    return ((uint32_t)reason < STD_ARRAY_SIZE(names)) ? names[reason] : "UNKNOWN";
}

void Mcu_PerformReset(void)
{
    HostSim_Exit(0, "MCU reset requested");
}

void HostSim_Exit(int code, const char *reason)
{
    (void)fprintf(stdout, "[host] %s (exit %d)\n", reason, code);
    (void)fflush(stdout);
    _exit(code);
}

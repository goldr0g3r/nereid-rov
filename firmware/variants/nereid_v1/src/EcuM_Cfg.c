/**
 * @file    EcuM_Cfg.c
 * @brief   Variant nereid_v1: start-up init lists (order matters).
 */
#include "BswM.h"
#include "Dem.h"
#include "Det.h"
#include "Dio.h"
#include "EcuM.h"
#include "Gpt.h"
#include "IoHwAb.h"
#include "Log.h"
#include "Mcu.h"
#include "Rte_Main.h"
#include "SchM.h"
#include "Uart.h"
#include "Variant_Cfg.h"
#include "WdgM.h"

void EcuM_Cfg_InitListZero(void)
{
    Mcu_Init();
    Det_Init();
    Gpt_Init();
    Dio_Init();
    Uart_Init();
    Log_Init();
    Dem_Init();
}

Std_ReturnType EcuM_Cfg_InitListOne(void)
{
    IoHwAb_Init(); /* outputs safe first */
    BswM_Init();
    if (Rte_Start() != E_OK)
    {
        return E_NOT_OK;
    }
    WdgM_Init(&WdgM_Config); /* starts the hardware watchdog */
    return SchM_Init();
}

const char *EcuM_Cfg_GetVariantName(void)
{
    return VARIANT_NAME " v" VARIANT_FIRMWARE_VERSION;
}

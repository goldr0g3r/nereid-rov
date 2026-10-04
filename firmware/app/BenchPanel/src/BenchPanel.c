/**
 * @file    BenchPanel.c
 * @brief   User-button press classification -> operator requests.
 */
#include "BenchPanel.h"

#include "Rte_BenchPanel.h"

static const BenchPanel_ConfigType *BenchPanel_Cfg;
static uint32_t                     BenchPanel_HeldMs;
static bool                         BenchPanel_WasPressed;
static bool                         BenchPanel_LongFired;

static Rte_OperatorRequestType BenchPanel_ShortPressRequest(Rte_VehicleModeType mode)
{
    switch (mode)
    {
    case RTE_VEHMODE_STANDBY:
        return RTE_OPREQ_ARM;
    case RTE_VEHMODE_ARMED_MANUAL:
    case RTE_VEHMODE_ARMED_ASSISTED:
    case RTE_VEHMODE_ARMED_DEGRADED:
        return RTE_OPREQ_DISARM;
    case RTE_VEHMODE_FAILSAFE:
    case RTE_VEHMODE_EMERGENCY_STOP:
        return RTE_OPREQ_RESET;
    default:
        return RTE_OPREQ_NONE;
    }
}

void BenchPanel_Init(const BenchPanel_ConfigType *config)
{
    BenchPanel_Cfg        = config;
    BenchPanel_HeldMs     = 0u;
    BenchPanel_WasPressed = false;
    BenchPanel_LongFired  = false;
}

void BenchPanel_MainFunction(void)
{
    bool pressed = false;

    (void)Rte_Read_BenchPanel_UserButton(&pressed);

    if (pressed)
    {
        if (BenchPanel_WasPressed)
        {
            BenchPanel_HeldMs += BenchPanel_Cfg->periodMs;
        }
        else
        {
            BenchPanel_HeldMs = 0u; /* press edge */
        }
        if (!BenchPanel_LongFired && (BenchPanel_HeldMs >= BenchPanel_Cfg->longPressMs))
        {
            BenchPanel_LongFired = true;
            (void)Rte_Write_BenchPanel_OperatorRequest(RTE_OPREQ_ESTOP);
        }
    }
    else if (BenchPanel_WasPressed)
    {
        /* release edge */
        if (!BenchPanel_LongFired && (BenchPanel_HeldMs >= BenchPanel_Cfg->minPressMs))
        {
            Rte_VehicleModeType     mode = RTE_VEHMODE_SAFE;
            Rte_OperatorRequestType req;

            (void)Rte_Read_BenchPanel_Mode(&mode);
            req = BenchPanel_ShortPressRequest(mode);
            if (req != RTE_OPREQ_NONE)
            {
                (void)Rte_Write_BenchPanel_OperatorRequest(req);
            }
        }
        BenchPanel_LongFired = false;
        BenchPanel_HeldMs    = 0u;
    }
    else
    {
        /* idle */
    }
    BenchPanel_WasPressed = pressed;
}

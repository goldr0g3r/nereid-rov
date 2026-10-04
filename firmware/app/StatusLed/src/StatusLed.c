/**
 * @file    StatusLed.c
 * @brief   Mode / fault indication on the green and red LEDs.
 *
 * Each pattern is a 16-bit mask; bit 15 is shown first, one bit per 100 ms step.
 */
#include "StatusLed.h"

#include "Rte_StatusLed.h"

#define STATUSLED_STEPS 16u

/*                                         green    red    */
static const uint16_t StatusLed_Patterns[RTE_VEHMODE_COUNT][2] = {
    [RTE_VEHMODE_SAFE]           = {0xAAAAu, 0x0000u}, /* green fast blink: booting / self-test */
    [RTE_VEHMODE_STANDBY]        = {0xC000u, 0x0000u}, /* green short heartbeat: ready, disarmed */
    [RTE_VEHMODE_ARMED_MANUAL]   = {0xFFFFu, 0x0000u}, /* green solid: ARMED */
    [RTE_VEHMODE_ARMED_ASSISTED] = {0xFFF0u, 0x0000u}, /* green solid with gap: assisted */
    [RTE_VEHMODE_ARMED_DEGRADED] = {0xFFFFu, 0xFF00u}, /* green solid + red slow blink */
    [RTE_VEHMODE_FAILSAFE]       = {0x0000u, 0xF0F0u}, /* red blink */
    [RTE_VEHMODE_EMERGENCY_STOP] = {0x0000u, 0xFFFFu}, /* red solid */
};

#define STATUSLED_ADVISORY_BLIP 0x0003u /* short red blip at the end of the cycle */

static uint8_t StatusLed_Step;

uint16_t StatusLed_GetGreenPattern(Rte_VehicleModeType mode)
{
    return ((uint32_t)mode < (uint32_t)RTE_VEHMODE_COUNT) ? StatusLed_Patterns[mode][0] : 0x0000u;
}

uint16_t StatusLed_GetRedPattern(Rte_VehicleModeType mode, Rte_FaultClassType cls)
{
    uint16_t red;

    if ((uint32_t)mode >= (uint32_t)RTE_VEHMODE_COUNT)
    {
        return 0xFFFFu; /* unknown mode: show the alarm pattern */
    }
    red = StatusLed_Patterns[mode][1];
    if ((red == 0x0000u) && (cls != RTE_FAULT_NONE))
    {
        red = STATUSLED_ADVISORY_BLIP;
    }
    return red;
}

void StatusLed_Init(void)
{
    StatusLed_Step = 0u;
    (void)Rte_Write_StatusLed_GreenLed(false);
    (void)Rte_Write_StatusLed_RedLed(false);
}

void StatusLed_MainFunction(void)
{
    Rte_VehicleModeType mode = RTE_VEHMODE_SAFE;
    Rte_FaultClassType  cls  = RTE_FAULT_NONE;
    uint16_t            bit  = (uint16_t)(0x8000u >> StatusLed_Step);

    (void)Rte_Read_StatusLed_Mode(&mode);
    (void)Rte_Read_StatusLed_FaultClass(&cls);

    (void)Rte_Write_StatusLed_GreenLed((StatusLed_GetGreenPattern(mode) & bit) != 0u);
    (void)Rte_Write_StatusLed_RedLed((StatusLed_GetRedPattern(mode, cls) & bit) != 0u);

    StatusLed_Step = (uint8_t)((StatusLed_Step + 1u) % STATUSLED_STEPS);
}

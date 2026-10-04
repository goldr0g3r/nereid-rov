/**
 * @file    Dio_PBcfg.c
 * @brief   Variant nereid_v1 on NUCLEO-F412ZG: Dio channel -> GPIO pin.
 *          Pin mode/pull/speed are configured by CubeMX (Nereid-ROV.ioc), which acts
 *          as the AUTOSAR Port driver; this table only names the pins.
 */
#include "Dio_Stm32.h"
#include "main.h"

const Dio_Stm32PinType Dio_Stm32Pins[DIO_CHANNEL_COUNT] = {
    [DIO_CH_LED_GREEN]   = {LD1_GPIO_Port, LD1_Pin},
    [DIO_CH_LED_BLUE]    = {LD2_GPIO_Port, LD2_Pin},
    [DIO_CH_LED_RED]     = {LD3_GPIO_Port, LD3_Pin},
    [DIO_CH_USER_BUTTON] = {USER_Btn_GPIO_Port, USER_Btn_Pin},
};

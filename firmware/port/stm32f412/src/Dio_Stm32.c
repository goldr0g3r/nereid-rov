/**
 * @file    Dio_Stm32.c
 * @brief   STM32 MCAL: Dio on HAL GPIO. Pins are configured by CubeMX MX_GPIO_Init().
 */
#include "Dio_Stm32.h"

void Dio_Init(void)
{
}

Dio_LevelType Dio_ReadChannel(Dio_ChannelType channel)
{
    if (channel >= (Dio_ChannelType)DIO_CHANNEL_COUNT)
    {
        return STD_LOW;
    }
    return (HAL_GPIO_ReadPin(Dio_Stm32Pins[channel].port, Dio_Stm32Pins[channel].pin) == GPIO_PIN_SET) ? STD_HIGH
                                                                                                         : STD_LOW;
}

void Dio_WriteChannel(Dio_ChannelType channel, Dio_LevelType level)
{
    if (channel < (Dio_ChannelType)DIO_CHANNEL_COUNT)
    {
        HAL_GPIO_WritePin(Dio_Stm32Pins[channel].port, Dio_Stm32Pins[channel].pin,
                          (level == STD_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

Dio_LevelType Dio_FlipChannel(Dio_ChannelType channel)
{
    Dio_LevelType level = (Dio_ReadChannel(channel) == STD_HIGH) ? STD_LOW : STD_HIGH;
    Dio_WriteChannel(channel, level);
    return level;
}

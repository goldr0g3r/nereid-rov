/**
 * @file    Dio_Stm32.h
 * @brief   STM32 Dio: physical pin table type (filled by the variant's Dio_PBcfg.c).
 */
#ifndef DIO_STM32_H
#define DIO_STM32_H

#include "Dio.h"
#include "stm32f4xx_hal.h"

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t      pin;
} Dio_Stm32PinType;

extern const Dio_Stm32PinType Dio_Stm32Pins[DIO_CHANNEL_COUNT];

#endif /* DIO_STM32_H */

/**
 * @file    Dio.h
 * @brief   MCAL: digital input/output by logical channel.
 *
 * Channel identifiers come from the variant's Dio_Cfg.h; each port maps them to
 * physical pins (on STM32: variants/<rov>/stm32f412/Dio_PBcfg.c).
 */
#ifndef DIO_H
#define DIO_H

#include "Dio_Cfg.h"
#include "Std_Types.h"

typedef uint8_t Dio_ChannelType;
typedef uint8_t Dio_LevelType; /* STD_LOW / STD_HIGH */

void          Dio_Init(void);
Dio_LevelType Dio_ReadChannel(Dio_ChannelType channel);
void          Dio_WriteChannel(Dio_ChannelType channel, Dio_LevelType level);
Dio_LevelType Dio_FlipChannel(Dio_ChannelType channel);

#endif /* DIO_H */

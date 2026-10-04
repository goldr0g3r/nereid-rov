/**
 * @file    Dio_Host.c
 * @brief   Host MCAL: Dio. Pins are an array; inputs are driven by the stimulus thread.
 *          Set NEREID_TRACE_DIO=1 to print every output change.
 */
#include "Dio.h"
#include "HostSim.h"

#include <stdio.h>
#include <stdlib.h>

static volatile Dio_LevelType Dio_Levels[DIO_CHANNEL_COUNT];
static bool                   Dio_Trace;

void Dio_Init(void)
{
    uint32_t i;
    for (i = 0u; i < (uint32_t)DIO_CHANNEL_COUNT; i++)
    {
        Dio_Levels[i] = STD_LOW;
    }
    Dio_Trace = (getenv("NEREID_TRACE_DIO") != NULL);
}

Dio_LevelType Dio_ReadChannel(Dio_ChannelType channel)
{
    return (channel < (Dio_ChannelType)DIO_CHANNEL_COUNT) ? Dio_Levels[channel] : STD_LOW;
}

void Dio_WriteChannel(Dio_ChannelType channel, Dio_LevelType level)
{
    if (channel < (Dio_ChannelType)DIO_CHANNEL_COUNT)
    {
        if (Dio_Trace && (Dio_Levels[channel] != level))
        {
            (void)printf("[host] dio %u = %u\n", (unsigned)channel, (unsigned)level);
        }
        Dio_Levels[channel] = level;
    }
}

Dio_LevelType Dio_FlipChannel(Dio_ChannelType channel)
{
    Dio_LevelType level = (Dio_ReadChannel(channel) == STD_HIGH) ? STD_LOW : STD_HIGH;
    Dio_WriteChannel(channel, level);
    return level;
}

void HostSim_SetDioLevel(Dio_ChannelType channel, Dio_LevelType level)
{
    if (channel < (Dio_ChannelType)DIO_CHANNEL_COUNT)
    {
        Dio_Levels[channel] = level;
    }
}

/* Test stub: Dio (pins are an array the test can read and drive). */
#include "Dio.h"

Dio_LevelType Stub_DioLevels[DIO_CHANNEL_COUNT];

void Dio_Init(void) {}
Dio_LevelType Dio_ReadChannel(Dio_ChannelType channel) { return Stub_DioLevels[channel]; }
void Dio_WriteChannel(Dio_ChannelType channel, Dio_LevelType level) { Stub_DioLevels[channel] = level; }
Dio_LevelType Dio_FlipChannel(Dio_ChannelType channel)
{
    Stub_DioLevels[channel] = (Stub_DioLevels[channel] == STD_HIGH) ? STD_LOW : STD_HIGH;
    return Stub_DioLevels[channel];
}

/**
 * @file    Uart_Host.c
 * @brief   Host MCAL: the debug channel is stdout. RX is not simulated yet (M2: socket/pty).
 */
#include "Uart.h"

#include <stdio.h>

void Uart_Init(void)
{
}

Std_ReturnType Uart_Write(Uart_ChannelType channel, const uint8_t *data, uint32_t len)
{
    if ((channel >= UART_CHANNEL_COUNT) || (data == NULL))
    {
        return E_NOT_OK;
    }
    (void)fwrite(data, 1u, len, stdout);
    (void)fflush(stdout);
    return E_OK;
}

uint32_t Uart_Read(Uart_ChannelType channel, uint8_t *data, uint32_t maxLen)
{
    STD_UNUSED(channel);
    STD_UNUSED(data);
    STD_UNUSED(maxLen);
    return 0u;
}

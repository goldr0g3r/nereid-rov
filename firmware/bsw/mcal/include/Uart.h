/**
 * @file    Uart.h
 * @brief   MCAL: serial byte streams by logical channel.
 *
 * TX is blocking with a bounded timeout and must only be called from low-priority
 * contexts (the Log/Com main functions). RX is interrupt driven into a ring buffer.
 */
#ifndef UART_H
#define UART_H

#include "Std_Types.h"
#include "Uart_Cfg.h" /* variant: UART_CH_* logical channels, UART_CHANNEL_COUNT */

typedef uint8_t Uart_ChannelType;

void           Uart_Init(void);
Std_ReturnType Uart_Write(Uart_ChannelType channel, const uint8_t *data, uint32_t len);
/** Non-blocking: copies up to @p maxLen received bytes, returns the count. */
uint32_t Uart_Read(Uart_ChannelType channel, uint8_t *data, uint32_t maxLen);

#endif /* UART_H */

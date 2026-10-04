/**
 * @file    Uart_Stm32.c
 * @brief   STM32 MCAL: UART. Debug channel = USART3 (ST-LINK VCP, 115200 8N1,
 *          initialised by CubeMX MX_USART3_UART_Init()).
 *
 * TX: blocking HAL transmit with timeout (called from the lowest-priority task only).
 * RX: RXNE interrupt into a ring buffer. Do NOT enable the USART3 global interrupt in
 *     CubeMX, this file owns USART3_IRQHandler.
 */
#include "Uart.h"

#include "RingBuf.h"
#include "stm32f4xx_hal.h"

#define UART_RX_BUFFER_SIZE 256u
#define UART_TX_TIMEOUT_MS  100u
#define UART_IRQ_PRIORITY   6u /* below configMAX_SYSCALL_INTERRUPT_PRIORITY */

extern UART_HandleTypeDef huart3; /* defined by CubeMX in main.c */

static uint8_t      Uart_RxStorage[UART_RX_BUFFER_SIZE];
static RingBuf_Type Uart_Rx;

void Uart_Init(void)
{
    (void)RingBuf_Init(&Uart_Rx, Uart_RxStorage, UART_RX_BUFFER_SIZE);
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
    HAL_NVIC_SetPriority(USART3_IRQn, UART_IRQ_PRIORITY, 0u);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
}

Std_ReturnType Uart_Write(Uart_ChannelType channel, const uint8_t *data, uint32_t len)
{
    if ((channel != UART_CH_DEBUG) || (data == NULL) || (len > 0xFFFFu))
    {
        return E_NOT_OK;
    }
    return (HAL_UART_Transmit(&huart3, (uint8_t *)data, (uint16_t)len, UART_TX_TIMEOUT_MS) == HAL_OK) ? E_OK
                                                                                                         : E_NOT_OK;
}

uint32_t Uart_Read(Uart_ChannelType channel, uint8_t *data, uint32_t maxLen)
{
    if ((channel != UART_CH_DEBUG) || (data == NULL))
    {
        return 0u;
    }
    return RingBuf_Read(&Uart_Rx, data, maxLen);
}

void USART3_IRQHandler(void)
{
    uint32_t sr = USART3->SR;

    if ((sr & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE)) != 0u)
    {
        uint8_t byte = (uint8_t)USART3->DR; /* reading SR then DR clears the flags */
        if ((sr & USART_SR_RXNE) != 0u)
        {
            (void)RingBuf_Put(&Uart_Rx, byte);
        }
    }
}

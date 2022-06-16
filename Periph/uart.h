// code = utf-8
#ifndef __uart_h
#define __uart_h
#include "stm32f10x_conf.h"

#define uartRxBufferSize 16

#define uartStopSymbol '\r' // 通信末尾标志

/**
 * @brief 串口接收缓冲区
 *
 */
extern uint8_t uartRxBuffer[uartRxBufferSize];

/**
 * @brief 缓冲区使用计数
 *
 */
extern uint8_t uartRxBufferIdx;

/**
 * @brief 缓冲区覆写标记
 *
 */
extern uint8_t uartRxBufferDirtyFlag;

void uartInit(uint32_t baudRate);
uint8_t uartSendChar(USART_TypeDef *commPort, uint8_t Char);
void uartSend(USART_TypeDef *commPort, uint8_t *Str);

#endif

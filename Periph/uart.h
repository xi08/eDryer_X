// code = utf-8
#ifndef __uart_h
#define __uart_h

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include "bitband_base.h"

#define uartRxBufferSize 16

#define uartStopSymbol '\r' // 通信末尾标志

/**
 * @brief 串口接收缓冲区
 *
 */
extern char uartRxBuffer[3][uartRxBufferSize];
#define uart1RxBuffer uartRxBuffer[0]
#define uart3RxBuffer uartRxBuffer[2]

/**
 * @brief 缓冲区使用计数
 *
 */
extern uint8_t uartRxBufferIdx[3];

/**
 * @brief 缓冲区覆写标记
 *
 */
extern uint8_t uartRxBufferDirtyFlag;
#define uart1RxBufferDirtyFlag bitBandAddr(&uartRxBufferDirtyFlag, 0)
#define uart3RxBufferDirtyFlag bitBandAddr(&uartRxBufferDirtyFlag, 2)

void uartInit(uint32_t baudRate);
char uartSendChar(USART_TypeDef *commPort, char Char);
void uartSend(USART_TypeDef *commPort, char *Str);
void uart3Init(uint32_t baudRate);



#endif

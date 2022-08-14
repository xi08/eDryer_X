// code = utf-8
#ifndef __wlan_h
#define __wlan_h

#include "stm32f10x.h" 
#include "stm32f10x_conf.h"

void wlanInit(const char* _ip,const uint16_t _port);
#define wlanSend(_Str) uartSend(USART3, _Str)
#define wlanRxBuffer uart3RxBuffer
#define wlanRxBufferDirtyFlag uart3RxBufferDirtyFlag
#endif

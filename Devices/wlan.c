
#include "wlan.h"
#include "delay.h"
#include "systime.h"
#include "uart.h"

#include <stdio.h>
#include <string.h>

void wlanInit(const char *_ip, const uint16_t _port)
{
    char tmp[45];
    uart3Init(115200);
    uartSend(USART3, "at\r\n");
    delay1ms(5);
    uartSend(USART3, "at+cipmode=1\r\n");
    delay1ms(5);
    sprintf(tmp, "at+cipstart=\"TCP\",%s,%u\r\n", _ip, _port);
    uartSend(USART3, tmp);
    delay1ms(5);
    uartSend(USART3, "at+cipsend\r\n");
    memset(wlanRxBuffer, 0, sizeof(wlanRxBuffer));
    wlanRxBufferDirtyFlag = 0;
    uartRxBufferIdx[2] = 0;
}

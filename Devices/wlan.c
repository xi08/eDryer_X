
#include "wlan.h"
#include "systime.h"
#include "uart.h"

void wlanInit(void)
{
    uart2Init(115200);
    uartSend(USART2, "AT");
    
}
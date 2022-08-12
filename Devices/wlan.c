
#include "wlan.h"
#include "systime.h"
#include "uart.h"

void wlanInit(void)
{
    uart3Init(115200);
    uartSend(USART3, "AT");
    
    
}
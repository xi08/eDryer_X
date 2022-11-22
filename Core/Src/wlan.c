#include "wlan.h"
#include <string.h>

#if WLAN_TYPE_ESP01S
const char apMode[] = "AT+CWMODE=1\r\n";

#endif

/*
HAL_StatusTypeDef WLAN_Init(UART_HandleTypeDef *huart)
{
    uint8_t retryTime = 5;
    char cmd[16];
    HAL_StatusTypeDef uartStatus;

#if WLAN_TYPE_ESP01S
    cmd = "at+rst\r\n";
    uartStatus = HAL_UART_Transmit(huart, (uint8_t *)cmd, strlen(cmd), 10);
    if(uartStatus) return uartStatus;
    HAL_Delay(100);


#endif
}
*/

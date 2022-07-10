// code = utf-8

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include "delay.h"
#include "systime.h"
#include "dbg.h"

#include "i2c.h"
#include "uart.h"

#include "at24c02.h"
#include "flash_rom.h"
#include "key.h"
#include "led.h"

#include <stdio.h>
#include <stdlib.h>

#include "IQmathLib.h"

uint16_t flashCache[2];
uint8_t e2promCache;
#define flashSaveAddr 0x08004000

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    sysTimeInit();
    delay1us_init();

    uartInit(115200);
    i2cInit();

    ledInit();
    keyInit();

    printf("%s\n%s:%s,%s\n", "MiniSTM32 V3.1", "Compiled in", __TIME__, __DATE__);
    led0Pin = 1;

    delay1ms(500);
    iwdgInit();

    while (1)
    {
        updateKey();
        keyProg();
        flashCache[0] = sysTime >> 16;
        flashCache[1] = sysTime;
        e2promCache = sysTime / 1000;
        led0Pin ^= 1;
        led1Pin ^= 1;
        delay1ms(50);
    }
}

/**
 * @brief 按键响应
 *
 */
void keyProg(void)
{
    switch (keyState[0]) // KEY0
    {
    case key_S1: // 短按
    {
        printf("key0S\n");
        flashROM_write(flashSaveAddr, flashCache, 2);
        printf("Flash Write: %u\n", ((uint32_t)(flashCache[0] << 16) + flashCache[1]));
        keyState[0] = key_S0;
        break;
    }
    case key_S3: // 长按
    {
        printf("key0L\n");
        at24c02_write(0, e2promCache);
        printf("E2PROM Write: %u\n", e2promCache);
        keyState[0] = key_S0;
        break;
    }
    default:
        break;
    }

    switch (keyState[1]) // KEY1
    {
    case key_S1: // 短按
    {
        printf("key1S\n");
        flashROM_read(flashSaveAddr, flashCache, 2);
        printf("Flash Read: %u\n", ((uint32_t)(flashCache[0] << 16) + flashCache[1]));
        keyState[1] = key_S0;
        break;
    }
    case key_S3: // 长按
    {
        printf("key1L\n");
        at24c02_read(0, &e2promCache);
        printf("E2PROM Read: %u\n", e2promCache);
        keyState[1] = key_S0;
        break;
    }
    default:
        break;
    }
}

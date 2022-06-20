// code = utf-8

#include "delay.h"
#include "systime.h"

#include "i2c.h"
#include "uart.h"

#include "at24c02.h"
#include "flash_rom.h"
#include "key.h"
#include "led.h"

#include <stdio.h>
#include <stdlib.h>

uint8_t led1Status;
uint16_t flashCache[2];
uint8_t e2promCache;
#define flashSaveAddr 0x08004000

int main()
{
    sysTimeInit();
    delay2us_init();
    uartInit(115200);

    led1Init();
    keyInit();
    i2cInit();

    printf("mini stm32f1 uart test");
    while (1)
    {
        updateKey();
        keyProg();
        flashCache[0] = sysTime >> 16;
        flashCache[1] = sysTime;
        e2promCache = sysTime / 1000;
        led1Status ^= 1;
        led1Control(led1Status);
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
        e2promCache = at24c02_read(0);
        printf("E2PROM Read: %u\n", e2promCache);
        keyState[1] = key_S0;
        break;
    }

    default:
        break;
    }
}

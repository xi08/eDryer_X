// code = utf-8

#include "delay.h"
#include "flash_rom.h"
#include "key.h"
#include "led.h"
#include "systime.h"
#include "uart.h"

#include <stdio.h>
#include <stdlib.h>

uint8_t led1Status;
uint16_t flashCache[2];
#define flashSaveAddr 0x08004000

int main()
{
    sysTimeInit();
    uartInit(115200);

    led1Init();
    keyInit();

    printf("mini stm32f1 uart test");
    while (1)
    {
        updateKey();
        keyProg();
        flashCache[0] = sysTime >> 16;
        flashCache[1] = sysTime;
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
        printf("key0S");
        flashROM_write(flashSaveAddr, flashCache, 2);
        printf("Write: %u\n", ((uint32_t)(flashCache[0] << 16) + flashCache[1]));
        keyState[0] = key_S0;
        break;
    }
    case key_S3: // 长按
    {
        printf("key0L");

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
        printf("key1S");
        flashROM_read(flashSaveAddr, flashCache, 2);
        printf("Read: %u\n", ((uint32_t)(flashCache[0] << 16) + flashCache[1]));
        keyState[1] = key_S0;
        break;
    }
    case key_S3: // 长按
    {
        printf("key1L");
        keyState[1] = key_S0;
        break;
    }

    default:
        break;
    }
}

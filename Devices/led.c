// code = utf-8

#include "led.h"
#include "bitband_io.h"

#define led1PinOut PDxOut(2)

/**
 * @brief 初始化绿色LED1
 *
 */
void led1Init(void)
{
    GPIO_InitTypeDef initSturct_gpio;

    /* 时钟设置 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); // 使能GPIOD时钟

    /* GPIO配置 */
    initSturct_gpio.GPIO_Pin = GPIO_Pin_2;        // 设置引脚号
    initSturct_gpio.GPIO_Speed = GPIO_Speed_2MHz; // 设置输出速度
    initSturct_gpio.GPIO_Mode = GPIO_Mode_Out_PP; // 设置普通推挽输出
    GPIO_Init(GPIOD, &initSturct_gpio);           // 配置引脚

    /* 电平设置 */
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, (BitAction)1); // 拉高电平关闭LED
}

/**
 * @brief LED1控制
 * 
 * @param status 控制信号
 */
void led1Control(uint8_t status)
{
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, (BitAction)(!status));
}

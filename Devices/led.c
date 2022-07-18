// code = utf-8

#include "led.h"

/**
 * @brief 初始化LED0/LED1
 *
 */
void led_init(void)
{
    GPIO_InitTypeDef initSturct_gpio;

    /* 时钟设置 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); // 使能GPIOD时钟

    /* GPIO配置 */
    initSturct_gpio.GPIO_Speed = GPIO_Speed_2MHz; // 设置输出速度
    initSturct_gpio.GPIO_Mode = GPIO_Mode_Out_PP; // 设置普通推挽输出
    initSturct_gpio.GPIO_Pin = GPIO_Pin_2;        // 设置引脚号
    GPIO_Init(GPIOD, &initSturct_gpio);           // 配置引脚
    initSturct_gpio.GPIO_Pin = GPIO_Pin_8;        // 设置引脚号
    GPIO_Init(GPIOA, &initSturct_gpio);           // 配置引脚

    /* 电平设置 */
    PAxOut(8) = 1; // 拉高电平关闭LED0
    PDxOut(2) = 1; // 拉高电平关闭LED1
}
/**
 * @brief 红色LED0控制
 *
 * @param status 控制信号
 */
void led0_control(uint8_t status)
{
    PAxOut(8) = (!status);
}

/**
 * @brief 绿色LED1控制
 *
 * @param status 控制信号
 */
void led1_control(uint8_t status)
{
    PDxOut(2) = (!status);
}

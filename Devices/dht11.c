// code = utf-8

#include "dht11.h"
#include "bitband_io.h"
#include "delay.h"

#define DHT11DataIn PAxIn(DHT11DataPin)
#define DHT11DataOut PAxOut(DHT11DataPin)

int8_t DHT11_tem;
int8_t DHT11_hum;

/**
 * @brief 初始化DHT11
 *
 */
void DHT11_init(void)
{
    GPIO_InitTypeDef initStruct_gpio;

    /* 时钟设置 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟

    /* GPIO配置 */
    initStruct_gpio.GPIO_Pin = 1 << DHT11DataPin;  // 设置引脚号
    initStruct_gpio.GPIO_Speed = GPIO_Speed_50MHz; // 设置输出速度
    initStruct_gpio.GPIO_Mode = GPIO_Mode_Out_OD;  // 设置普通开漏输出
    GPIO_Init(GPIOA, &initStruct_gpio);            // 配置引脚

    /* 电平设置 */
    DHT11DataOut = 1; // 释放总线
}

/**
 * @brief DHT11发送开始信号并检测设备
 *
 * @return uint8_t 错误码
 */
uint8_t DHT11_start(void)
{
    DHT11DataOut = 0;    // 拉低总线，发出检测信号
    delay1ms(20);        // 稳定电平
    DHT11DataOut = 1;    // 释放总线
    delay1us(40);        // 等待电平稳定中点
    if (DHT11DataIn)     // DHT11无响应信号
        return 1;        // 报错弹出
    while (!DHT11DataIn) // 等待DHT11释放总线
        ;
    return 0; // 正常退出
}

/**
 * @brief DHT11接收单字节
 *
 * @return uint8_t 读出数据
 */
uint8_t DHT11_readByte(void)
{
    uint8_t i;
    uint8_t rData;

    for (i = 0; i < 8; i++)
    {

        while (DHT11DataIn) // 等待总线被占用
            ;
        rData <<= 1;         // 数据左移
        while (!DHT11DataIn) // 等待数字信号低电平结束
            ;
        delay1us(36);         // 等待到数字信号'1'高电平的中点
        rData |= DHT11DataIn; // 未被拉低则是信号'1'，被拉低则上一位信号是'0'
    }

    return rData;
}

/**
 * @brief
 *
 * @return uint8_t
 */
uint8_t DHT11_read(void)
{
    uint8_t i;
    uint8_t dat[4];   // 数据
    uint8_t checkSum; // 校验和

    /* 数据读出 */
    if (DHT11_start()) // 发送开始信号并检测设备
        return 1;
    for (i = 0; i < 4; i++)
        dat[i] = DHT11_readByte();
    checkSum = DHT11_readByte();

    /* 数据校验 */
    if (checkSum != (dat[0] + dat[1] + dat[2] + dat[3]))
        return 2;

    /* 数据转换 */
    DHT11_hum = (int8_t)dat[0]; // 湿度，5~95
    DHT11_tem = (int8_t)dat[2]; // 温度，-20~60
    if (dat[3] & 128)           //温度为负
        DHT11_tem *= (-1);
    return 0;
}

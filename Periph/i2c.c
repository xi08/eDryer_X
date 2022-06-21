// code = utf-8
#include "i2c.h"
#include "bitband_io.h"
#include "delay.h"

#define i2cSDAIn PCxIn(i2cSDAPin)
#define i2cSCLIn PCxIn(i2cSCLPin)
#define i2cSDAOut PCxOut(i2cSDAPin)
#define i2cSCLOut PCxOut(i2cSCLPin)

#ifdef i2c_HS
#define i2cDelay() delay1us(2)
#else
#define i2cDelay() delay1us(9)
#endif

/**
 * @brief 发送启动信号
 *
 */
void i2cSTART(void)
{
    i2cSDAOut = 1; // 释放数据线
    i2cSCLOut = 1; // 释放时钟线
    i2cDelay();    // 稳定电平
    i2cSDAOut = 0; // 发出开始信号
    i2cDelay();    // 稳定电平
    i2cSCLOut = 0; // 钳住时钟线
    i2cDelay();    // 稳定电平
}

/**
 * @brief 发送结束信号
 *
 */
void i2cSTOP(void)
{
    i2cSDAOut = 0; // 准备发出信号
    i2cSCLOut = 1; // 释放时钟线
    i2cDelay();    // 稳定电平
    i2cSDAOut = 1; // 发出结束信号并释放数据线
}

/**
 * @brief 发送应答信号
 *
 */
void i2cACK(void)
{
    i2cSDAOut = 0; // 发出应答信号
    i2cDelay();    // 稳定电平
    i2cSCLOut = 1; // 产生时钟
    i2cDelay();    // 稳定电平
    i2cSCLOut = 0; // 产生时钟
    i2cDelay();    // 稳定电平
    i2cSCLOut = 1; // 释放时钟线
}

/**
 * @brief 发送非应答信号
 *
 */
void i2cNACK(void)
{
    i2cSDAOut = 1; // 发出非应答信号
    i2cDelay();    // 稳定电平
    i2cSCLOut = 1; // 产生时钟
    i2cDelay();    // 稳定电平
    i2cSCLOut = 0; // 产生时钟
    i2cDelay();    // 稳定电平
}

/**
 * @brief 初始化i2c总线
 *
 */
void i2cInit(void)
{
    GPIO_InitTypeDef initStruct_gpio;

    /* 时钟设置 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // 使能GPIOC时钟

    /* GPIO配置 */
    initStruct_gpio.GPIO_Pin = (1 << i2cSCLPin) | (1 << i2cSDAPin); // 设置引脚号
    initStruct_gpio.GPIO_Speed = GPIO_Speed_50MHz;                  // 设置输出速度
    initStruct_gpio.GPIO_Mode = GPIO_Mode_Out_OD;                   // 设置普通开漏输出
    GPIO_Init(GPIOC, &initStruct_gpio);                             // 配置引脚

    /* 电平设置 */
    i2cSCLOut = 1; // 释放时钟线
    i2cSDAOut = 1; // 释放数据线
}

/**
 * @brief i2c死锁复位
 *
 */
void i2cReset(void)
{
    uint8_t i;
    /* 构造9个时钟 */
    for (i = 0; i < 9; i++)
    {

        i2cSCLOut = 1; // 释放时钟线
        i2cDelay();    // 稳定电平
        i2cSCLOut = 0; // 钳住时钟线
        i2cDelay();    // 稳定电平
    }
    i2cSCLOut = 1; // 释放时钟线
}

/**
 * @brief 发送单字节
 *
 * @param sData 发送数据
 */
void i2cSend(uint8_t sData)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        i2cSCLOut = 0;                        // 钳住时钟线
        i2cSDAOut = ((sData & 0x80) ? 1 : 0); // 发出数据高位
        sData <<= 1;                          // 数据左移
        i2cDelay();                           // 稳定电平
        i2cSCLOut = 1;                        // 释放时钟线，表明数据有效
        i2cDelay();                           // 稳定电平
    }
    i2cSCLOut = 0; // 钳住时钟线
    i2cSDAOut = 1; // 释放数据线
    i2cDelay();    // 稳定电平
}

/**
 * @brief 读出单字节
 *
 * @return uint8_t 读出数据
 */
uint8_t i2cRead(void)
{
    uint8_t i;
    uint8_t rData;

    for (i = 0; i < 8; i++)
    {
        rData <<= 1;                 // 数据左移
        i2cSCLOut = 1;               // 释放时钟线，表明数据有效
        i2cDelay();                  // 稳定电平
        rData |= (i2cSDAIn ? 1 : 0); // 读出数据高位
        i2cSCLOut = 0;               // 钳住时钟线
        i2cDelay();                  // 稳定电平
    }
    return rData;
}

/**
 * @brief 读出应答信号
 *
 * @return uint8_t 应答信号电平
 */
uint8_t i2cReadACK(void)
{
    uint8_t ackSig;

    i2cSDAOut = 1;               // 释放数据线
    i2cDelay();                  // 稳定电平
    i2cSCLOut = 1;               // 释放时钟线，准备读出应答信号
    i2cDelay();                  // 稳定电平
    ackSig = (i2cSDAIn ? 1 : 0); // 读出应答信号
    i2cSCLOut = 0;               // 钳住时钟线
    i2cDelay();                  // 稳定电平
    return ackSig;
}

/**
 * @brief 设备存在检测
 *
 * @param addr 设备地址
 * @return uint8_t 设备响应信号
 */
uint8_t i2cAddrCheck(uint8_t addr)
{
    uint8_t ackSig;

    i2cSTART();            // 发出启动信号
    i2cSend(addr | 0);     // 发送设备地址
    ackSig = i2cReadACK(); // 读出设备应答
    i2cSTOP();             // 发出结束信号

    return ackSig;
}

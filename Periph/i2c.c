// code = utf-8
#include "i2c.h"
#include "bitband_io.h"
#include "delay.h"

#define i2cSDAIn PCxIn(i2cSDAPin)
#define i2cSCLIn PCxIn(i2cSCLPin)
#define i2cSDAOut PCxOut(i2cSDAPin)
#define i2cSCLOut PCxOut(i2cSCLPin)

#ifdef i2cMaster_SpeedMode400KHz
#define i2cDelay() delay1us(2) // O2以上优化下约400KHz速度
#else
#define i2cDelay() delay1us(9) // 约100KHz速度
#endif

i2cState_enum i2cState;

/**
 * @brief 初始化双向i2c总线
 *
 */
void i2cInit(void)
{
    GPIO_InitTypeDef initStruct_gpio;
    EXTI_InitTypeDef initStruct_exti;
    NVIC_InitTypeDef initStruct_nvic;

    /* 时钟设置 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // 使能GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // 使能IO复用时钟

    /* GPIO配置 */
    initStruct_gpio.GPIO_Pin = (1 << i2cSCLPin) | (1 << i2cSDAPin); // 设置引脚号
    initStruct_gpio.GPIO_Speed = GPIO_Speed_50MHz;                  // 设置输出速度
    initStruct_gpio.GPIO_Mode = GPIO_Mode_Out_OD;                   // 设置普通开漏输出
    GPIO_Init(GPIOC, &initStruct_gpio);                             // 配置引脚

    /* 电平控制 */
    i2cSCLOut = 1; // 释放时钟线
    i2cSDAOut = 1; // 释放数据线

    /* IO复用设置 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, i2cSCLPin); // 设置时钟线外部中断
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, i2cSDAPin); // 设置数据线外部中断

    /* 引脚中断设置 */
    initStruct_exti.EXTI_Line = ((uint32_t)(1 << i2cSCLPin) | (uint32_t)(1 << i2cSDAPin)); // 设置引脚号
    initStruct_exti.EXTI_Mode = EXTI_Mode_Interrupt;                                       // 设置触发中断
    initStruct_exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;                            // 设置双边沿触发
    initStruct_exti.EXTI_LineCmd = ENABLE;                                                 // 使能中断线
    EXTI_Init(&initStruct_exti);                                                           // 配置引脚中断

    /* 中断设置 */
    initStruct_nvic.NVIC_IRQChannel = EXTI15_10_IRQn;      // 设置中断号
    initStruct_nvic.NVIC_IRQChannelCmd = ENABLE;           // 设置中断使能状态
    initStruct_nvic.NVIC_IRQChannelPreemptionPriority = 0; // 设置抢占优先级
    initStruct_nvic.NVIC_IRQChannelSubPriority = 3;        // 设置子优先级
    NVIC_Init(&initStruct_nvic);                           // 设置中断
}

/**
 * @brief 进入i2c主机模式
 *
 */
void i2cMasterModeEnable(void)
{
    /* 引脚中断设置 */
    (EXTI->IMR) &= ~((uint32_t)(1 << i2cSCLPin) | (uint32_t)(1 << i2cSDAPin)); // 屏蔽对应引脚中断请求

    /* 模式设置 */
    i2cState = i2cMasterState; // 切入主机模式
}

/**
 * @brief i2c主机模式退出
 *
 */
void i2cMasterModeDisable(void)
{
    /* 模式设置 */
    i2cState = i2cIdleState; // 切出主机模式

    /* 引脚中断设置 */
    (EXTI->IMR) |= ((uint32_t)(1 << i2cSCLPin) | (uint32_t)(1 << i2cSDAPin)); // 开放对应引脚中断请求
}

/**
 * @brief 主机-发送启动信号
 *
 */
void i2cSTART(void)
{
    /* 工作状态检测 */
    if (i2cState != i2cMasterState)
        return;

    /* 电平控制 */
    i2cSDAOut = 1; // 释放数据线
    i2cSCLOut = 1; // 释放时钟线
    i2cDelay();    // 稳定电平
    i2cSDAOut = 0; // 发出开始信号
    i2cDelay();    // 稳定电平
    i2cSCLOut = 0; // 钳住时钟线
    i2cDelay();    // 稳定电平
}

/**
 * @brief 主机-发送结束信号
 *
 */
void i2cSTOP(void)
{
    /* 工作状态检测 */
    if (i2cState != i2cMasterState)
        return;

    /* 电平控制 */
    i2cSDAOut = 0; // 准备发出信号
    i2cSCLOut = 1; // 释放时钟线
    i2cDelay();    // 稳定电平
    i2cSDAOut = 1; // 发出结束信号并释放数据线
    i2cDelay();    // 稳定电平
}

/**
 * @brief 主机-发送应答信号
 *
 */
void i2cACK(void)
{
    /* 工作状态检测 */
    if (i2cState != i2cMasterState)
        return;

    /* 电平控制 */
    i2cSDAOut = 0; // 发出应答信号
    i2cDelay();    // 稳定电平
    i2cSCLOut = 1; // 产生时钟
    i2cDelay();    // 稳定电平
    i2cSCLOut = 0; // 产生时钟
    i2cDelay();    // 稳定电平
    i2cSCLOut = 1; // 释放时钟线
}

/**
 * @brief 主机-发送非应答信号
 *
 */
void i2cNACK(void)
{
    /* 工作状态检测 */
    if (i2cState != i2cMasterState)
        return;

    /* 电平控制 */
    i2cSDAOut = 1; // 发出非应答信号
    i2cDelay();    // 稳定电平
    i2cSCLOut = 1; // 产生时钟
    i2cDelay();    // 稳定电平
    i2cSCLOut = 0; // 产生时钟
    i2cDelay();    // 稳定电平
}

/**
 * @brief 主机-i2c死锁复位
 *
 */
void i2cReset(void)
{
    uint8_t i;

    /* 工作状态检测 */
    if (i2cState != i2cMasterState)
        return;

    /* 电平控制 */
    for (i = 0; i < 9; i++)
    {
        /* 构造9个时钟 */
        i2cSCLOut = 1; // 释放时钟线
        i2cDelay();    // 稳定电平
        i2cSCLOut = 0; // 钳住时钟线
        i2cDelay();    // 稳定电平
    }
    i2cSCLOut = 1; // 释放时钟线

    /* 模式切换 */
    i2cState = i2cIdleState; // 切出主机模式
}

/**
 * @brief 主机-发送单字节
 *
 * @param sData 发送数据
 */
void i2cSend(uint8_t sData)
{
    uint8_t i;
    /* 工作状态检测 */
    if (i2cState != i2cMasterState)
        return;

    /* 电平控制 */
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
 * @brief 主机-读出单字节
 *
 * @return uint8_t 读出数据
 */
uint8_t i2cRead(void)
{
    uint8_t i, rData;

    /* 工作状态检测 */
    if (i2cState != i2cMasterState)
        return 0xff;

    /* 电平控制 */
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
 * @brief 主机-读出应答信号
 *
 * @return uint8_t 应答信号电平
 */
uint8_t i2cReadACK(void)
{
    uint8_t ackSig;

    /* 工作状态检测 */
    if (i2cState != i2cMasterState)
        return 1;

    /* 电平控制 */
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
 * @brief 主机-设备存在检测
 *
 * @param addr 设备地址
 * @return uint8_t 设备响应信号
 */
uint8_t i2cAddrCheck(uint8_t addr)
{
    uint8_t ackSig;

    /* 工作状态检测 */
    if (i2cState != i2cMasterState)
        return 1;

    /* 电平控制 */
    i2cSTART();                // 发出启动信号
    i2cSend(addr | i2cAddrWR); // 发送设备写地址
    ackSig = i2cReadACK();     // 读出设备应答
    i2cSTOP();                 // 发出结束信号

    return ackSig;
}

/**
 * @brief i2c引脚中断响应
 *
 */
void i2cEXTIResp(void)
{
    if (!EXTI_GetITStatus((uint32_t)(1 << i2cSCLPin)) | (uint32_t)(1 << i2cSDAPin)) // i2c对应引脚电平变化
    {
        EXTI_ClearITPendingBit(((uint32_t)(1 << i2cSCLPin)) | (uint32_t)(1 << i2cSDAPin)); // 清除中断标记
    }
}

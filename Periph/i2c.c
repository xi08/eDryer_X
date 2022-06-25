// code = utf-8
#include "i2c.h"
#include "bitband_base.h"
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

#define i2cMaster_SCLStretchingTimeMax 20 // 主机最大时钟拉伸数
#define i2cSlave_MaxCommTime 1000         // 从机最大通讯时长(ms)

i2cState_enum i2cState;

uint8_t i2cSlaveAddr = 0x92; // 从机模式监听地址, 8位

/**
 * @brief 从机数据收发控制位：
 *  0：收/#发；
 *  1：地址/#数据；
 *
 */
uint8_t i2cDataCFG;

uint8_t i2cSlaveBitCnt;         // 从机收发数据位计数
uint8_t i2cSlaveRx, i2cSlaveTx; // 从机收发数据缓冲

uint32_t i2cSlaveCommStartTimestamp; // 从机进入时间戳

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
    initStruct_gpio.GPIO_Pin = ((uint32_t)(1 << i2cSCLPin) | (uint32_t)(1 << i2cSDAPin)); // 设置引脚号
    initStruct_gpio.GPIO_Speed = GPIO_Speed_50MHz;                                        // 设置输出速度
    initStruct_gpio.GPIO_Mode = GPIO_Mode_Out_OD;                                         // 设置普通开漏输出
    GPIO_Init(GPIOC, &initStruct_gpio);                                                   // 配置引脚

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
 * @brief 更改i2c从机地址
 *
 * @param newAddr 新i2c地址
 */
void i2cSlaveAddrChange(uint8_t newAddr)
{
    i2cSlaveAddr = newAddr;
}

/**
 * @brief 进入i2c主机模式
 *
 * @return i2cStatusCode_enum i2c工作状态
 */
i2cStatusCode_enum i2cMasterModeEnable(void)
{
    /* 工作状态检测 */
    if (i2cState != i2cIdleState) // 非空闲
        return i2cStatusCode_ModeErr;

    /* 引脚中断设置 */
    (EXTI->IMR) &= ~((uint32_t)(1 << i2cSCLPin) | (uint32_t)(1 << i2cSDAPin)); // 屏蔽对应引脚中断请求

    /* 模式设置 */
    i2cState = i2cMasterState; // 切入主机模式

    return i2cStatusCode_Success;
}

/**
 * @brief 退出i2c主机模式
 *
 * @return i2cStatusCode_enum i2c工作状态
 */
i2cStatusCode_enum i2cMasterModeDisable(void)
{
    /* 工作状态检测 */
    if (i2cState != i2cMasterState) // 非主机模式
        return i2cStatusCode_ModeErr;

    /* 模式设置 */
    i2cState = i2cIdleState; // 切出主机模式

    /* 引脚中断设置 */
    (EXTI->IMR) |= ((uint32_t)(1 << i2cSCLPin) | (uint32_t)(1 << i2cSDAPin)); // 开放对应引脚中断请求

    return i2cStatusCode_Success;
}

/**
 * @brief 主机-发送启动信号
 *
 * @return i2cStatusCode_enum i2c工作状态
 */
i2cStatusCode_enum i2cSTART(void)
{
    /* 工作状态检测 */
    if (i2cState != i2cMasterState) // 非主机模式
        return i2cStatusCode_ModeErr;

    /* 数据线控制 */
    i2cSDAOut = 1; // 释放数据线

    /* 数据线回读 */
    if (i2cSDAIn != i2cSDAOut) // 总线控制丢失
        return i2cStatusCode_LostBusCtrl;

    /* 时钟线控制 */
    i2cSCLOut = 1; // 释放时钟线
    i2cDelay();    // 稳定电平

    /* 数据线控制 */
    i2cSDAOut = 0; // 发出开始信号
    i2cDelay();    // 稳定电平

    /* 数据线回读 */
    if (i2cSDAIn != i2cSDAOut) // 总线控制丢失
        return i2cStatusCode_LostBusCtrl;

    /* 时钟线控制 */
    i2cSCLOut = 0; // 钳住时钟线
    i2cDelay();    // 稳定电平

    return i2cStatusCode_Success;
}

/**
 * @brief 主机-发送结束信号
 *
 * @return i2cStatusCode_enum i2c工作状态
 */
i2cStatusCode_enum i2cSTOP(void)
{
    /* 工作状态检测 */
    if (i2cState != i2cMasterState) // 非主机模式
        return i2cStatusCode_ModeErr;

    /* 数据线控制 */
    i2cSDAOut = 0; // 准备发出信号

    /* 数据线回读 */
    if (i2cSDAIn != i2cSDAOut) // 总线控制丢失
        return i2cStatusCode_LostBusCtrl;

    /* 时钟线控制 */
    i2cSCLOut = 1; // 释放时钟线
    i2cDelay();    // 稳定电平

    /* 数据线控制 */
    i2cSDAOut = 1; // 发出结束信号并释放数据线

    /* 数据线回读 */
    if (i2cSDAIn != i2cSDAOut) // 总线控制丢失
        return i2cStatusCode_LostBusCtrl;

    return i2cStatusCode_Success;
}

/**
 * @brief 主机-发送应答信号
 *
 * @return i2cStatusCode_enum i2c工作状态
 */
i2cStatusCode_enum i2cACK(void)
{
    /* 工作状态检测 */
    if (i2cState != i2cMasterState) // 非主机模式
        return i2cStatusCode_ModeErr;

    /* 时钟线控制 */
    i2cSCLOut = 0; // 钳住时钟线，保证数据可变
    i2cDelay();    // 稳定电平

    /* 数据线控制 */
    i2cSDAOut = 0; // 发出应答信号

    /* 数据线回读 */
    if (i2cSDAIn != i2cSDAOut) // 总线控制丢失
        return i2cStatusCode_LostBusCtrl;

    /* 时钟线控制 */
    i2cSCLOut = 1; // 释放时钟线，保证数据有效
    i2cDelay();    // 稳定电平

    /* 时钟线控制 */
    i2cSCLOut = 0; // 钳住时钟线，保证数据可变
    i2cDelay();    // 稳定电平

    /* 数据线控制 */
    i2cSDAOut = 1; // 释放数据线

    /* 数据线回读 */
    if (i2cSDAIn != i2cSDAOut) // 总线控制丢失
        return i2cStatusCode_LostBusCtrl;

    return i2cStatusCode_Success;
}

/**
 * @brief 主机-发送非应答信号
 *
 * @return i2cStatusCode_enum i2c工作状态
 */
i2cStatusCode_enum i2cNACK(void)
{
    /* 工作状态检测 */
    if (i2cState != i2cMasterState) // 非主机模式
        return i2cStatusCode_ModeErr;

    /* 时钟线控制 */
    i2cSCLOut = 0; // 钳住时钟线，保证数据可变
    i2cDelay();    // 稳定电平

    /* 数据线控制 */
    i2cSDAOut = 1; // 发出非应答信号

    /* 数据线回读 */
    if (i2cSDAIn != i2cSDAOut) // 总线控制丢失
        return i2cStatusCode_LostBusCtrl;

    /* 时钟线控制 */
    i2cSCLOut = 1; // 释放时钟线，保证数据有效
    i2cDelay();    // 稳定电平

    /* 时钟线控制 */
    i2cSCLOut = 0; // 钳住时钟线，保证数据可变
    i2cDelay();    // 稳定电平

    /* 数据线控制 */
    i2cSDAOut = 1; // 释放数据线

    /* 数据线回读 */
    if (i2cSDAIn != i2cSDAOut) // 总线控制丢失
        return i2cStatusCode_LostBusCtrl;

    return i2cStatusCode_Success;
}

/**
 * @brief 主机-发送单字节
 *
 * @param sData 发送数据
 * @return i2cStatusCode_enum i2c工作状态
 */
i2cStatusCode_enum i2cSend(uint8_t sData)
{
    uint8_t i;

    /* 工作状态检测 */
    if (i2cState != i2cMasterState) // 非主机模式
        return i2cStatusCode_ModeErr;

    /* 连续8位数据 */
    for (i = 0; i < 8; i++)
    {
        /* 时钟线控制 */
        i2cSCLOut = 0; // 钳住时钟线，保证数据可变
        i2cDelay();    // 稳定电平

        /* 数据线控制 */
        i2cSDAOut = ((sData & 0x80) ? 1 : 0); // 发出数据高位

        /* 数据线回读 */
        if (i2cSDAIn != i2cSDAOut) // 总线控制丢失
            return i2cStatusCode_LostBusCtrl;

        /* 数据位控制 */
        sData <<= 1; // 数据左移

        /* 时钟线控制 */
        i2cSCLOut = 1; // 释放时钟线，保证数据有效
        i2cDelay();    // 稳定电平
    }

    /* 时钟线控制 */
    i2cSCLOut = 0; // 钳住时钟线，保证数据可变
    i2cDelay();    // 稳定电平

    /* 数据线控制 */
    i2cSDAOut = 1; // 释放数据线

    /* 数据线回读 */
    if (i2cSDAIn != i2cSDAOut) // 总线控制丢失
        return i2cStatusCode_LostBusCtrl;

    return i2cStatusCode_Success;
}

/**
 * @brief 主机-读出单字节
 *
 * @param rData 读出指针
 * @return i2cStatusCode_enum i2c工作状态
 */
i2cStatusCode_enum i2cRead(uint8_t *rData)
{
    uint8_t i;

    /* 工作状态检测 */
    if (i2cState != i2cMasterState) // 非主机模式
        return i2cStatusCode_ModeErr;

    /* 连续8位数据 */
    for (i = 0; i < 8; i++)
    {
        /* 数据位控制 */
        (*rData) <<= 1; // 数据左移

        /* 时钟线控制 */
        i2cSCLOut = 1; // 释放时钟线，保证数据有效
        i2cDelay();    // 稳定电平

        /* 数据线控制 */
        (*rData) |= (i2cSDAIn ? 1 : 0); // 读出数据高位

        /* 时钟线控制 */
        i2cSCLOut = 0; // 钳住时钟线
        i2cDelay();    // 稳定电平
    }

    return i2cStatusCode_Success;
}

/**
 * @brief 主机-读出应答信号
 *
 * @return i2cStatusCode_enum i2c工作状态
 */
i2cStatusCode_enum i2cReadACK(void)
{
    uint8_t ackSig;
    uint8_t SCLStretchingTime = i2cMaster_SCLStretchingTimeMax;

    /* 工作状态检测 */
    if (i2cState != i2cMasterState) // 非主机模式
        return i2cStatusCode_ModeErr;

    /* 数据线控制 */
    i2cSDAOut = 1; // 释放数据线

    /* 时钟线控制 */
    i2cSCLOut = 1; // 释放时钟线，保证数据有效
    i2cDelay();    // 稳定电平

    /* 时钟线回读 */
    while (!i2cSCLIn) // 进入时钟拉伸
    {
        if (!SCLStretchingTime) // 时钟拉伸过长
            return i2cStatusCode_Timeout;
        SCLStretchingTime--;
        i2cDelay();
    }

    /* 数据线控制 */
    ackSig = (i2cSDAIn ? 1 : 0); // 读出应答信号

    /* 时钟线控制 */
    i2cSCLOut = 0; // 钳住时钟线，保证数据可变
    i2cDelay();    // 稳定电平

    return (i2cStatusCode_enum)ackSig;
}

/**
 * @brief 主机-i2c死锁复位
 *
 * @return i2cStatusCode_enum i2c工作状态
 */
i2cStatusCode_enum i2cReset(void)
{
    uint8_t i;

    /* 工作状态检测 */
    if (i2cState != i2cMasterState) // 非主机模式
        return i2cStatusCode_ModeErr;

    /* 构造9个时钟 */
    for (i = 0; i < 9; i++)
    {
        /* 时钟线控制 */
        i2cSCLOut = 1; // 释放时钟线
        i2cDelay();    // 稳定电平

        /* 时钟线控制 */
        i2cSCLOut = 0; // 钳住时钟线
        i2cDelay();    // 稳定电平
    }
    i2cSCLOut = 1; // 释放时钟线

    return i2cStatusCode_Success;
}

/**
 * @brief 主机-设备存在检测
 *
 * @param addrWR 设备写地址
 * @return i2cStatusCode_enum i2c工作状态
 */
i2cStatusCode_enum i2cAddrCheck(uint8_t addrWR)
{
    i2cStatusCode_enum statusCode;

    /* 工作状态检测 */
    if (i2cState != i2cMasterState) // 非主机模式
        return i2cStatusCode_ModeErr;

    /* 发起通信 */
    statusCode = i2cSTART(); // 发出启动信号
    if (statusCode > 1)      // 存在错误
        return statusCode;

    /* 发送地址 */
    statusCode = i2cSend(addrWR); // 发送设备写地址
    if (statusCode > 1)           // 存在错误
        return statusCode;

    /* 接收应答 */
    statusCode = i2cReadACK(); // 读出设备应答
    if (statusCode > 1)        // 存在错误
        return statusCode;

    /* 结束通信 */
    statusCode = i2cSTOP(); // 发出结束信号
    return statusCode;
}

/**
 * @brief i2c从机响应
 *
 */
void i2cSlaveModeResp(void)
{
    if (!EXTI_GetITStatus((uint32_t)(1 << i2cSCLPin))) // 时钟线电平变化
    {
        if (!i2cSCLIn) // SCL低电平，数据可变；本机发送
        {
            switch (i2cState)
            {
            case i2cStartState: // 准备数据位接收
            {
                /* 数据控制 */
                i2cSlaveBitCnt = 0; // 清空数据位计数
                i2cSlaveRx = 0;     // 清空接收缓冲
                i2cSlaveTx = 0;     // 清空输出缓冲

                /* 模式切换 */
                bitBandAddr((&i2cDataCFG), 0) = 1; // 选择为接收
                bitBandAddr((&i2cDataCFG), 1) = 1; // 选择为地址
                i2cState = i2cDataState;           // 切入数据收发模式

                break;
            }
            case i2cDataState: // 数据位
            {

                break;
            }
            case i2cACKState: // 应答位
            {
                if (bitBandAddr((&i2cDataCFG), 0)) // 接收结束，本机发送
                {
                    i2cSDAOut = 0;                     // 发送应答位
                    bitBandAddr((&i2cDataCFG), 1) = 0; // 选择为数据
                    i2cState = i2cDataState;           // 再次切入数据收发模式
                }

                break;
            }
            default: // 非法状态
                break;
            }
        }
        else // SCL高电平，数据不可变；本机接收
        {
            switch (i2cState)
            {
            case i2cDataState: // 数据位
            {
                if (bitBandAddr((&i2cDataCFG), 0)) // 接收控制
                {
                    if (bitBandAddr((&i2cDataCFG), 1)) // 地址接收
                    {
                        if (i2cSlaveBitCnt < 8) // 地址未接收完
                        {
                            i2cSlaveRx |= i2cSDAIn; // 接收数据位
                            i2cSlaveRx <<= 1;       // 数据左移
                            i2cSlaveBitCnt++;       // 数据计数自增
                        }
                        else // 地址已接收完，进行比对
                        {
                            if (i2cSlaveRx == (i2cSlaveAddr | i2cAddrWR)) // 比对正确，为写地址
                            {
                                bitBandAddr((&i2cDataCFG), 0) = 1; // 选择为接收
                                i2cState = i2cACKState;            // 切入应答收发模式
                            }
                            else if (i2cSlaveRx == (i2cSlaveAddr | i2cAddrRD)) // 比对正确，为读地址
                            {
                                bitBandAddr((&i2cDataCFG), 0) = 0; // 选择为发送
                                i2cState = i2cACKState;            // 切入应答收发模式
                            }
                            else // 比对失败
                            {
                                i2cState = i2cStopState; // 锁定,等待当前通信结束或超时
                            }
                            i2cSlaveBitCnt = 0; // 清空数据计数
                        }
                    }
                    else // 数据接收
                    {
                    }
                }
                break;
            }
            case i2cACKState: // 应答位
            {
                break;
            }
            default: // 非法状态
                break;
            }
        }
        EXTI_ClearITPendingBit(((uint32_t)(1 << i2cSCLPin))); // 清除中断标记
    }
    else if (!EXTI_GetITStatus((uint32_t)(1 << i2cSDAPin))) // 数据线电平变化
    {
        if (!i2cSDAIn) // SDA下降沿
        {
            if (i2cSCLIn)
                i2cState = i2cStartState; // 接收到启动信号
        }
        else // SDA上升沿
        {
            if (i2cSCLIn)
                i2cState = i2cIdleState; // 接收到结束信号
        }

        EXTI_ClearITPendingBit(((uint32_t)(1 << i2cSDAPin))); // 清除中断标记
    }
}

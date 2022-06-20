// code = utf-8
#include "i2c.h"
#include "delay.h"

#define i2cSDAOutput(x) GPIO_WriteBit(GPIOC, GPIO_Pin_11, (BitAction)(x))
#define i2cSCLOutput(x) GPIO_WriteBit(GPIOC, GPIO_Pin_12, (BitAction)(x))
#define i2cSDAInput() GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11)
#define i2cDelay() delay2us(5)

/**
 * @brief 初始化i2c总线
 *
 */
void i2cInit(void)
{
    GPIO_InitTypeDef initStruct_gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // 使能GPIOC时钟
    initStruct_gpio.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12; // 设置引脚号
    initStruct_gpio.GPIO_Speed = GPIO_Speed_10MHz; // 设置输出速度
    initStruct_gpio.GPIO_Mode = GPIO_Mode_Out_OD; // 设置普通开漏输出
    GPIO_Init(GPIOC, &initStruct_gpio); // 配置引脚
}

/**
 * @brief 切换输出模式
 *
 */
void i2cOutputMode(void)
{
    GPIO_InitTypeDef i2cGPIO;
    i2cGPIO.GPIO_Pin = GPIO_Pin_11;// 设置引脚号
    i2cGPIO.GPIO_Speed = GPIO_Speed_10MHz;// 设置输出速度
    i2cGPIO.GPIO_Mode = GPIO_Mode_Out_PP;// 设置普通开漏输出
    GPIO_Init(GPIOC, &i2cGPIO);// 配置引脚
}

/**
 * @brief 切换输入模式
 *
 */
void i2cInputMode(void)
{
    GPIO_InitTypeDef i2cGPIO;
    i2cGPIO.GPIO_Pin = GPIO_Pin_11;// 设置引脚号
    i2cGPIO.GPIO_Mode = GPIO_Mode_IPU;// 设置浮空输入
    GPIO_Init(GPIOC, &i2cGPIO);// 配置引脚
}

/**
 * @brief 总线复位
 *
 */
void i2cReset(void)
{
    uint8_t i;
    for (i = 0; i < 9; i++)
    {
        i2cSCLOutput(1);
        i2cDelay();
        i2cSCLOutput(0);
        i2cDelay();
        if (i2cSDAInput())
            return;
    }
}

/**
 * @brief 发送启动信号
 *
 */
void i2cSTART(void)
{
    i2cSDAOutput(1);
    i2cSCLOutput(1);
    i2cDelay();
    i2cSDAOutput(0);
    i2cDelay();
    i2cSCLOutput(0);
}

/**
 * @brief 发送结束信号
 *
 */
void i2cSTOP(void)
{
    i2cSDAOutput(0);
    i2cSCLOutput(1);
    i2cDelay();
    i2cSDAOutput(1);
    i2cDelay();
    i2cSCLOutput(0);
}

/**
 * @brief 发送应答信号
 *
 */
void i2cACK(void)
{
    i2cSDAOutput(0);
    i2cDelay();
    i2cSCLOutput(1);
    i2cDelay();
    i2cSCLOutput(0);
    i2cDelay();
    i2cSDAOutput(1);
}

/**
 * @brief 发送非应答信号
 *
 */
void i2cNACK(void)
{
    i2cSDAOutput(1);
    i2cDelay();
    i2cSCLOutput(1);
    i2cDelay();
    i2cSCLOutput(0);
    i2cDelay();
}

/**
 * @brief 等待回复ACK信号
 *
 * @return uint8_t 运行状态
 */
uint8_t i2cWaitACK(void)
{
    uint8_t errTime = 5;
    i2cInputMode();
    i2cDelay();
    i2cSCLOutput(1);
    i2cDelay();
    while (i2cSDAInput())
    {
        errTime--;
        i2cDelay();
        if (!errTime)
        {
            i2cOutputMode();
            i2cSTOP();
            return 1;
        }
    }
    i2cSCLOutput(0);
    i2cDelay();
    i2cOutputMode();
    return 0;
}

/**
 * @brief 发送单字节
 *
 * @param dat 需要发送的数据
 */
void i2cSend(uint8_t dat)
{
    uint8_t i;
    i2cOutputMode();
    for (i = 0; i < 8; i++)
    {
        i2cSCLOutput(0);
        i2cDelay();
        i2cSDAOutput(dat & 0x80);
        dat <<= 1;
        i2cDelay();
        i2cSCLOutput(1);
        i2cDelay();
    }
    i2cSCLOutput(0);
    i2cDelay();
}

/**
 * @brief 接受单字节
 *
 * @return uint8_t 接收到的数据
 */
uint8_t i2cReceive(void)
{
    uint8_t i;
    uint8_t data = 0;
    i2cInputMode();
    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        i2cSCLOutput(1);
        i2cDelay();
        data |= i2cSDAInput();
        i2cSCLOutput(0);
        i2cDelay();
    }
    i2cOutputMode();
    return data;
}

// code = utf-8

#include "dht11.h"
#include "bitband_io.h"
#include "delay.h"

#define DHT11_In() (PAi(0))
#define DHT11_Out(x) (PAo(0) = x)

/**
 * @brief DHT11-温度数据
 *
 */
uint8_t dht11_temDataInt, dht11_temDataDec;

/**
 * @brief DHT11-湿度数据
 *
 */
uint8_t dht11_humDataInt, dht11_humDataDec;

/**
 * @brief DHT11-输入模式
 *
 */
void DHT11_OutputMode(void)
{
    GPIO_InitTypeDef DHT11_GPIO;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 开启时钟
    DHT11_GPIO.GPIO_Mode = GPIO_Mode_Out_PP;              // 推挽输出
    DHT11_GPIO.GPIO_Speed = GPIO_Speed_50MHz;             // 频率50Mhz
    DHT11_GPIO.GPIO_Pin = GPIO_Pin_0;                     // 设置引脚
    GPIO_Init(GPIOA, &DHT11_GPIO);                        // 初始化引脚
}

/**
 * @brief DHT11-输出模式
 *
 */
void DHT11_InputMode(void)
{
    GPIO_InitTypeDef DHT11_GPIO;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 开启时钟
    DHT11_GPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;         // 浮空输入
    DHT11_GPIO.GPIO_Speed = GPIO_Speed_50MHz;             // 频率50Mhz
    DHT11_GPIO.GPIO_Pin = GPIO_Pin_0;                     // 设置引脚
    GPIO_Init(GPIOA, &DHT11_GPIO);                        // 初始化引脚
}

/**
 * @brief DHT11-初始化设备
 *
 */
void DHT11_Init(void)
{
    /* GPIO设置 */
    GPIO_InitTypeDef DHT11_GPIO;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 开启时钟
    DHT11_GPIO.GPIO_Mode = GPIO_Mode_Out_PP;              // 推挽输出
    DHT11_GPIO.GPIO_Speed = GPIO_Speed_50MHz;             // 频率50Mhz
    DHT11_GPIO.GPIO_Pin = GPIO_Pin_0;                     // 设置引脚
    GPIO_Init(GPIOA, &DHT11_GPIO);                        // 初始化引脚
}
/**
 * @brief DHT11-发送起始信号并检测响应
 *
 * @return uint8_t 错误码
 */
uint8_t DHT11_Start(void)
{
    DHT11_OutputMode(); // 输出模式
    DHT11_Out(0);       // 拉低
    delay1ms(20);       // 等待20ms
    DHT11_InputMode();  // 输入模式
    delay2us(20);       // 等待40us
    if (DHT11_In())     // 响应(低电平部分)检测
        return 1;       // 电平错误，退出
    delay2us(25);       // 等待50us
    while (DHT11_In())  // 等待响应(高电平部分)结束
        ;
    return 0;
}
/**
 * @brief DHT11-接收单字节
 *
 * @return uint8_t 接收的数据
 */
uint8_t DHT11_ReadByte(void)
{
    uint8_t i;
    uint8_t dat; // 数据暂存
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        delay2us(39);      // 等待54+24us
        dat |= DHT11_In(); // 若仍为高电平，则为信号'1'
        while (DHT11_In()) // 等待下次下拉
            ;
    }
    return dat;
}

/**
 * @brief DHT11-读出数据
 *
 * @return uint8_t 错误码
 */
uint8_t DHT11_Read(void)
{
    uint8_t i;
    uint8_t dat[4];     // 数据
    uint8_t checkSum;   // 校验和
    uint8_t datSum = 0; // 数据和

    /* 读出数据 */
    if (DHT11_Start())
        return 1;
    for (i = 0; i < 4; i++)
    {
        dat[i] = DHT11_ReadByte();
        datSum += dat[i];
    }

    /* 进行校验 */
    checkSum = DHT11_ReadByte();
    if (checkSum != datSum)
        return 1;

    /* 数据格式转换 */
    dht11_temDataInt = dat[0];
    dht11_temDataDec = dat[1];
    dht11_humDataInt = dat[2];
    dht11_humDataDec = dat[3];
    return 0;
}

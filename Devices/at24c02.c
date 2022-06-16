// code = utf-8

#include "at24c02.h"
#include "i2c.h"
#include "sysTime.h"
/**
 * @brief AT24C02初始化
 *
 */
void at24c02_init(void)
{
    i2cInit();
}

/**
 * @brief 单字节写
 *
 * @param destin 目标地址
 * @param dat 写入的数据
 */
void at24c02_write(uint8_t destin, uint8_t dat)
{
    i2cSTART();

    i2cSend((at24c02_addr << 1) | 0);
    i2cWaitACK();

    i2cSend(destin);
    i2cWaitACK();

    i2cSend(dat);
    i2cWaitACK();

    i2cSTOP();
    delay1ms(6);
}
/**
 * @brief 单字节读
 *
 * @param source 源地址
 * @return uint8_t 读出的数据
 */
uint8_t at24c02_read(uint8_t source)
{
    uint8_t data = 0;
    i2cSTART();

    i2cSend((at24c02_addr << 1) | 0);
    i2cWaitACK();

    i2cSend(source);
    i2cWaitACK();

    i2cSTART();

    i2cSend((at24c02_addr << 1) | 1);
    i2cWaitACK();

    data = i2cReceive();
    i2cACK();

    i2cSTOP();
    return data;
}

/**
 * @brief 页写
 *
 * @param destin 目标地址
 * @param source 源地址
 * @param n 数据大小
 */
void at24c02_writePage(uint8_t destin, uint8_t *source, uint8_t n)
{
    i2cSTART();
    i2cSend((at24c02_addr << 1) | 0);
    i2cWaitACK();
    i2cSend(destin);
    i2cWaitACK();
    while (n--)
    {
        i2cSend(*source++);
        i2cWaitACK();
    }
    i2cSTOP();

    delay1ms(6);
}

/**
 * @brief 页读
 *
 * @param source 源地址
 * @param destin 目标地址
 * @param n 数据大小
 */
void at24c02_readPage(uint8_t source, uint8_t *destin, uint8_t n)
{
    i2cSTART();
    i2cSend((at24c02_addr << 1) | 0);
    i2cWaitACK();
    i2cSend(source);
    i2cWaitACK();

    i2cSTART();
    i2cSend((at24c02_addr << 1) | 1);
    i2cWaitACK();
    while (n--)
    {
        *destin++ = i2cReceive();
        if (n)
            i2cACK();
        else
            i2cNACK();
    }
    i2cSTOP();
}

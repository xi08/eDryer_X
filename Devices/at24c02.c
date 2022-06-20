// code = utf-8

#include "at24c02.h"
#include "i2c.h"
#include "delay.h"
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

    i2cSend(at24c02_addr | 0);
    i2cReadACK();

    i2cSend(destin);
    i2cReadACK();

    i2cSend(dat);
    i2cReadACK();

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

    i2cSend(at24c02_addr | 0);
    i2cReadACK();

    i2cSend(source);
    i2cReadACK();

    i2cSTART();

    i2cSend(at24c02_addr | 1);
    i2cReadACK();

    data = i2cRead();
    i2cACK();

    i2cSTOP();
    return data;
}

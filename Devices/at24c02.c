// code = utf-8

#include "at24c02.h"
#include "delay.h"
#include "i2c.h"

/**
 * @brief 单字节写
 *
 * @param addr 片上写入地址
 * @param data 写入数据
 */
void at24c02_write(uint8_t addr, uint8_t data)
{
    i2cSTART();                // 发送启动信号
    i2cSend(at24c02_addr | 0); // 发送器件写地址
    i2cReadACK();              // 获取器件应答
    i2cSend(addr);             // 发送写入地址
    i2cReadACK();              // 获取器件应答
    i2cSend(data);             // 发送写入数据
    i2cReadACK();              // 获取器件应答
    i2cSTOP();                 // 发送结束信号
    delay1ms(6);               // 等待器件内部写入
}

/**
 * @brief 单字节读
 *
 * @param addr 片上读出地址
 * @return uint8_t 读出数据
 */
uint8_t at24c02_read(uint8_t addr)
{
    uint8_t data;

    i2cSTART();                // 发送启动信号
    i2cSend(at24c02_addr | 0); // 发送器件写地址
    i2cReadACK();              // 获取器件应答
    i2cSend(addr);             // 发送读出地址
    i2cReadACK();              // 获取器件应答

    i2cSTART();                // 第二次发送启动信号
    i2cSend(at24c02_addr | 1); // 发送器件读地址
    i2cReadACK();              // 获取器件应答
    data = i2cRead();          // 获取读出数据
    i2cACK();                  // 发送应答信号
    i2cSTOP();                 // 发送结束信号

    return data;
}

/**
 * @brief 多字节页写
 *
 * @param addr 片上写入首地址
 * @param data 写入数据指针
 * @param n 写入量
 */
void at24c02_writePage(uint8_t addr, uint8_t *data, uint8_t n)
{
    i2cSTART();                // 发送启动信号
    i2cSend(at24c02_addr | 0); // 发送器件写地址
    i2cReadACK();              // 获取器件应答
    i2cSend(addr);             // 发送写入首地址
    i2cReadACK();              // 获取器件应答
    while (n--)                // 写入计数
    {
        i2cSend(*data++); // 发送写入数据
        i2cReadACK();     // 获取器件应答
    }
    i2cSTOP();   // 发送结束信号
    delay1ms(6); // 等待器件内部写入
}

/**
 * @brief 多字节页读
 *
 * @param addr 片上读取首地址
 * @param data 读取数据指针
 * @param n 读取量
 */
void at24c02_readPage(uint8_t addr, uint8_t *data, uint8_t n)
{
    i2cSTART();                // 发送启动信号
    i2cSend(at24c02_addr | 0); // 发送器件写地址
    i2cReadACK();              // 获取器件应答
    i2cSend(addr);             // 发送读出首地址
    i2cReadACK();              // 获取器件应答

    i2cSTART();                // 第二次发送启动信号
    i2cSend(at24c02_addr | 1); // 发送器件读地址
    i2cReadACK();              // 获取器件应答
    while (n--)                // 读取计数
    {
        *data++ = i2cRead(); // 获取读出数据
        if (n)
            i2cACK(); // 发送应答信号，表示仍未读完
        else
            i2cNACK(); // 发送非应答信号，表示已读完
    }
    i2cSTOP(); // 发送结束信号
}

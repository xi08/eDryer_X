// code = utf-8

#include "flash_rom.h"

uint16_t flashBuffer[1024]; // 整页数据临时读出存储

/**
 * @brief Flash写入
 *
 * @param destAddr 目标Flash地址
 * @param src 源数据存储
 * @param n 写入数
 */
void flashWrite16b(uint32_t destAddr, uint16_t *src, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
    {
        FLASH_ProgramHalfWord(destAddr, src[i]);
        destAddr += 2;
    }
}

/**
 * @brief 以16b读出FLash
 *
 * @param srcAddr 源Flash地址
 * @param dest 目标数据存储
 * @param n 读取数
 */
void flashROM_read(uint32_t srcAddr, uint16_t *dest, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
    {
        dest[i] = *(volatile uint16_t *)srcAddr;
        srcAddr += 2;
    }
}

/**
 * @brief 以16b写入FLash
 *
 * @param destAddr 目标Flash地址
 * @param src 源数据存储
 * @param n 写入数
 */
void flashROM_write(uint32_t destAddr, uint16_t *src, size_t n)
{
    uint16_t i;

    uint32_t writeAddr;  // 写入地址相对偏移
    uint8_t pagePos;     // 写入页号
    uint16_t pageOff;    // 写入页内偏移
    uint16_t pageWriteN; // 页写入量

    /* 检测地址正确性 */
    if ((destAddr < FLASH_BASE) || (destAddr >= (FLASH_BASE + 128 * 2048)))
        return;

    /* 计算首个写入地址 */
    writeAddr = destAddr - FLASH_BASE;                            // 计算相对偏移
    pagePos = writeAddr >> 11;                                    // 计算页号
    pageOff = (writeAddr & 2047) >> 1;                            // 计算页内偏移
    pageWriteN = ((n > (1024 - pageOff)) ? (1024 - pageOff) : n); // 计算该页写入量

    /* 写入 */
    FLASH_Unlock(); // 解锁
    while (n)
    {
        /* 读出整页 */
        flashROM_read(FLASH_BASE + ((uint32_t)pagePos >> 11), flashBuffer, 1024);
        /* 写入区域空白检测 */
        for (i = 0; i < pageWriteN; i++)
            if (flashBuffer[pageOff + i] != 0xffff) // 非空
                break;
        /* 数据擦除与写入 */
        if (i < pageWriteN) // 需要擦除
        {
            FLASH_ErasePage(FLASH_BASE + ((uint32_t)pagePos << 11)); // 擦除整页
            for (i = 0; i < pageWriteN; i++)                         // 新数据覆盖
                flashBuffer[i + pageOff] = src[i];
            flashWrite16b(FLASH_BASE + ((uint32_t)pagePos << 11), flashBuffer, 1024); // 完整数据回写
        }
        else //不需要擦除, 仅写入新数据
            flashWrite16b(destAddr, src, pageWriteN);

        /* 写入结束检测 */
        n -= pageWriteN; // 刷新剩余写入量
        if (n)
        {
            pagePos++;                            // 更新至下一页
            pageOff = 0;                          // 重置页内偏移
            src += pageWriteN;                    // 更新源数据存储指针
            writeAddr += pageWriteN;              // 更新目标Flash地址
            pageWriteN = ((n > 1024) ? 1024 : n); // 刷新下一页写入量
        }
    }
    FLASH_Lock(); // 上锁
}

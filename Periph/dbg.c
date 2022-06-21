// code = utf-8
#include "dbg.h"

/**
 * @brief 关闭jtag调试
 *
 */
void jtagOff(void)
{
    // GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    // 复用时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // 关闭jtag
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

/**
 * @brief 关闭所有中断
 *
 */
void nvicASMDisable(void)
{
    __ASM volatile("cpsid i");
}

/**
 * @brief 开启所有中断
 *
 */
void nvicASMEnable(void)
{
    __ASM volatile("cpsie i");
}
    
/**
 * @brief 空指令
 * 
 */
void asmNOP(void)
{
    __ASM volatile("nop");
}

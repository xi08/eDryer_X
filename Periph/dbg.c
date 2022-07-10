// code = utf-8
#include "dbg.h"

/**
 * @brief 关闭jtag调试
 *
 */
void jtagDisable(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); // GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);                         // 复用时钟
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);                     // 关闭jtag
}

/**
 * @brief 初始化看门狗
 *
 */
void iwdgInit(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); // 使能对寄存器IWDG_PR和IWDG_RLR的写操作
    IWDG_SetPrescaler(IWDG_Prescaler_16);         // 设置IWDG预分频值
    IWDG_SetReload(2750);                         // 设置IWDG重装载值
    IWDG_ReloadCounter();                         // 按照IWDG重装载寄存器的值重装载IWDG计数器
    IWDG_Enable();                                // 使能IWDG
}

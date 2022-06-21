// code = utf-8
#include "sysTime.h"

/**
 * @brief 毫秒时基
 *
 */
volatile uint32_t sysTime;

/**
 * @brief 初始化系统时基
 *
 */
void sysTimeInit(void)
{
    sysTime = 0;                            // 清空计数器值
    SysTick_Config(SystemCoreClock / 1000); // 配置SysTick为1ms产生一次中断
    NVIC_SetPriority(SysTick_IRQn, 0);      // 配置SysTick优先于任何非内核中断
}

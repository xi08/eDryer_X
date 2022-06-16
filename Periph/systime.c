// code = utf-8
#include "sysTime.h"

/**
 * @brief 全局毫秒系统时基计数器
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
    NVIC_SetPriority(SysTick_IRQn, 1);      // 配置SysTick为最高中断优先
}

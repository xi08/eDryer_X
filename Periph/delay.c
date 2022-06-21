// code = utf-8
#include "delay.h"
#include "systime.h"

/**
 * @brief 硬件毫秒延迟
 *
 * @param t 延迟 t 毫秒
 */
void delay1ms(uint32_t t)
{
    uint32_t delay1msStartTimestamp = sysTime, delayTime = t;
    if (delayTime < 0xffffffff)
        delayTime++;
    while (sysTime - delay1msStartTimestamp < delayTime) // 等待
        ;
}

/**
 * @brief 硬件微秒延迟初始化
 *
 */
void delay1us_init(void)
{
    TIM_TimeBaseInitTypeDef initStruct_TIM;
    
    /* 时钟设置 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); // 使能TIM6时钟

    /* TIM配置 */
    initStruct_TIM.TIM_Period = 8;                       // 设置周期为9
    initStruct_TIM.TIM_Prescaler = 7;                    // 设置预分频为8
    initStruct_TIM.TIM_CounterMode = TIM_CounterMode_Up; // 设置为向上计数
    TIM_TimeBaseInit(TIM6, &initStruct_TIM);             // 写入对应寄存器
}

/**
 * @brief 硬件微秒延迟
 *
 * @param t 延迟 t 微秒
 */
void delay1us(uint8_t t)
{
    while (t--)
    {
        TIM_ClearFlag(TIM6, TIM_FLAG_Update);             // 清空TIM6更新标记
        TIM_Cmd(TIM6, ENABLE);                            // 使能TIM6
        while (!TIM_GetFlagStatus(TIM6, TIM_FLAG_Update)) // 等待
            ;
    }
}

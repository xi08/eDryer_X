// code = utf-8
#include "key.h"
#include "systime.h"

/**
 * @brief 等待确认状态进入时间戳
 *
 */
uint32_t keyPressTimestamp[keyNum];

/**
 * @brief 按键状态枚举
 *
 */
keyState_enum keyState[keyNum];

/**
 * @brief 按键初始化
 *
 */
void key_init(void)
{
    GPIO_InitTypeDef initStruct_GPIO;

    /* 时钟设置 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // 使能GPIOC时钟

    /* GPIO配置 */
    initStruct_GPIO.GPIO_Mode = GPIO_Mode_IPU; // 设置上拉输入
    initStruct_GPIO.GPIO_Pin = GPIO_Pin_5;     // 设置引脚号
    GPIO_Init(GPIOC, &initStruct_GPIO);        // 配置引脚
    initStruct_GPIO.GPIO_Pin = GPIO_Pin_15;    // 设置引脚号
    GPIO_Init(GPIOA, &initStruct_GPIO);        // 配置引脚
}
/**
 * @brief 更新按键信息
 *
 */
void key_updateInfo(void)
{
    uint8_t i = keyNum, keyInfo = 0xff;

    /* 获取按键信息 */
    keyInfo ^= (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) << 0);
    keyInfo ^= (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) << 1);

    /* 按键信息转换 */
    while (i--)
    {
        // 按键按下
        if (keyInfo & (1 << i))
        {
            switch (keyState[i])
            {
            case key_S0: // 处于等待按下状态
            {
                keyState[i] = key_S2;           // 状态切换
                keyPressTimestamp[i] = sysTime; // 更新时间戳
                break;
            }
            default:
                break;
            }
        }
        // 按键抬起
        if (!(keyInfo & (1 << i)))
        {
            switch (keyState[i])
            {
            case key_S2: // 处于等待抬起状态
            {
                if (sysTime - keyPressTimestamp[i] >= keyLongPressTime)       // 长按检测
                    keyState[i] = key_S3;                                     // 状态切换
                else if (sysTime - keyPressTimestamp[i] >= keyShortPressTime) // 短按检测
                    keyState[i] = key_S1;                                     // 状态切换
                else
                    keyState[i] = key_S0; // 状态清除
                break;
            }
            default: // 非法状态
            {
                keyState[i] = key_S0; // 状态清除
                break;
            }
            }
        }
    }
}

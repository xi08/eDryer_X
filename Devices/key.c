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
void keyInit(void)
{
    GPIO_InitTypeDef keyStruct_GPIO;

    // GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

    // 设置浮空输入
    keyStruct_GPIO.GPIO_Mode = GPIO_Mode_IPU;

    // 配置Key0
    keyStruct_GPIO.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOC, &keyStruct_GPIO);

    // 配置Key1
    keyStruct_GPIO.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &keyStruct_GPIO);
}
/**
 * @brief 更新按键信息
 *
 */
void updateKey(void)
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
            case key_S0:
                keyState[i] = key_S2;
                keyPressTimestamp[i] = sysTime;
                break;

            default:
                break;
            }
        }
        // 按键抬起
        if (!(keyInfo & (1 << i)))
        {
            switch (keyState[i])
            {
            case key_S2:
                if (sysTime - keyPressTimestamp[i] >= keyLongPressTime)
                    keyState[i] = key_S3;
                else if (sysTime - keyPressTimestamp[i] >= keyShortPressTime)
                    keyState[i] = key_S1;
                else
                    keyState[i] = key_S0;
                break;

            default:
                keyState[i] = key_S0;
                break;
            }
        }
    }
}

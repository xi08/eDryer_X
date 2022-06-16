// code = utf-8

#include "led.h"

void led1Init(void)
{
    GPIO_InitTypeDef gpioInitSturct_PD2;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    gpioInitSturct_PD2.GPIO_Pin = GPIO_Pin_2;
    gpioInitSturct_PD2.GPIO_Speed = GPIO_Speed_2MHz;
    gpioInitSturct_PD2.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &gpioInitSturct_PD2);
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, (BitAction)1);
}
void led1Control(uint8_t status)
{
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, (BitAction)(!status));
}

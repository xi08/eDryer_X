// code = utf-8
#ifndef __dht11_h
#define __dht11_h
#include "stm32f10x_conf.h"

/**
 * @brief 温度数据
 *
 */
extern uint8_t dht11_temDataInt, dht11_temDataDec;

/**
 * @brief 湿度数据
 *
 */
extern uint8_t dht11_humDataInt, dht11_humDataDec;

void DHT11_Init(void);
uint8_t DHT11_Start(void);
uint8_t DHT11_Read(void);

#endif

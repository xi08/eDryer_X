// code = utf-8
#ifndef __dht11_h
#define __dht11_h
#include "stm32f10x_conf.h"

#define dht11DataPin 0

/**
 * @brief 温度数据
 *
 */
extern int8_t dht11_tem;

/**
 * @brief 湿度数据
 *
 */
extern int8_t dht11_hum;

void dht11Init(void);
uint8_t dht11START(void);
uint8_t dht11Read(void);

#endif

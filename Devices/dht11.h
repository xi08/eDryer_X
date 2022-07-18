// code = utf-8
#ifndef __dht11_h
#define __dht11_h

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define DHT11DataPin 0

/**
 * @brief 温度数据
 *
 */
extern int8_t DHT11_tem;

/**
 * @brief 湿度数据
 *
 */
extern int8_t DHT11_hum;

void DHT11_init(void);
uint8_t DHT11_start(void);
uint8_t DHT11_read(void);

#endif

// code = utf-8
#ifndef __led_h
#define __led_h

#include "bitband_io.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define led0Pin PAxOut(8)
#define led1Pin PDxOut(2)

void ledInit(void);
void led0Control(uint8_t status);
void led1Control(uint8_t status);

#endif

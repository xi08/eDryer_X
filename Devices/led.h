// code = utf-8
#ifndef __led_h
#define __led_h

#include "bitband_io.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define led0Pin PAxOut(8)
#define led1Pin PDxOut(2)

void led_init(void);
void led0_control(uint8_t status);
void led1_control(uint8_t status);

#endif

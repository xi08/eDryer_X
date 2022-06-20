// code = utf-8
#ifndef __i2c_h
#define __i2c_h

#include "stm32f10x_conf.h"

#define i2cSCLPin 12
#define i2cSDAPin 11

void i2cInit(void);
void i2cSTART(void);
void i2cSTOP(void);
void i2cACK(void);
void i2cNACK(void);
void i2cSend(uint8_t sData);
void i2cReset(void);
uint8_t i2cRead(void);
uint8_t i2cReadACK(void);

#endif

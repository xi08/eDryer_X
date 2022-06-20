// code = utf-8
#ifndef __i2c_hs_h
#define __i2c_hs_h

#include "stm32f10x_conf.h"

#define i2cSCLPin_HS 12
#define i2cSDAPin_HS 11

void i2cInit_HS(void);
void i2cSend_HS(uint8_t sendData);
void i2cReset_HS(void);
uint8_t i2cRead_HS(void);
uint8_t i2cReadAck_HS(void);

#endif

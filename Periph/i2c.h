// code = utf-8
#ifndef __i2c_h
#define __i2c_h

#include "stm32f10x_conf.h"
#include "stm32f10x.h"

#define i2cSCLPin 12
#define i2cSDAPin 11

#define i2cAddrWR 0
#define i2cAddrRD 1

typedef enum
{
    i2cIdleState = 0,
    i2cMasterState,
    i2cStartState = 2,
    i2cDataState,
    i2cACKState,
    i2cNACKState,
    i2cStopState,

} i2cState_enum;
extern i2cState_enum i2cState;

void i2cInit(void);
void i2cMasterModeEnable(void);
void i2cMasterModeDisable(void);
void i2cEXTIResp(void);

void i2cSTART(void);
void i2cSTOP(void);
void i2cACK(void);
void i2cNACK(void);
void i2cSend(uint8_t sData);
void i2cReset(void);
uint8_t i2cRead(void);
uint8_t i2cReadACK(void);

#endif

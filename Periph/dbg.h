// code = utf-8
#ifndef __dbg_h
#define __dbg_h
#include "stm32f10x_conf.h"

void jtagOff(void);
void nvicASMDisable(void);
void nvicASMEnable(void);
void asmNOP(void);


#endif

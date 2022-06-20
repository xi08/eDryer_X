// code = utf-8
#ifndef __sysTime_h
#define __sysTime_h
#include "stm32f10x_conf.h"

/**
 * @brief 毫秒时基
 *
 */
extern volatile uint32_t sysTime;

void sysTimeInit(void);

#endif

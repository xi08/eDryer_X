// code = utf-8
#ifndef __sysTime_h
#define __sysTime_h
#include "stm32f10x_conf.h"

/**
 * @brief 全局毫秒计时计数器
 *
 */
extern volatile uint32_t sysTime;

void sysTimeInit(void);

#endif

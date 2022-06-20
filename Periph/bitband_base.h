// code = utf-8

#ifndef __bitband_base_h
#define __bitband_base_h

#include "stm32f10x.h"
#include <stddef.h>

#define bitBandTran(addr, pos) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (pos << 2))
#define bitBandAddr(addr, pos) (*((volatile size_t *)(bitBandTran(addr, pos))))

#endif

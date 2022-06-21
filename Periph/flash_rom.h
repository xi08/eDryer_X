// code = utf-8
#ifndef __flashrom_h
#define __flashrom_h

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <stddef.h>

void flashROM_read(uint32_t srcAddr, uint16_t *dest, size_t n);
void flashROM_write(uint32_t destAddr, uint16_t *src, size_t n);

#endif

// code = utf-8
#ifndef at24c02_h
#define at24c02_h

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define at24c02_addr 0xa0

void at24c02_init(void);
void at24c02_write(uint8_t addr, uint8_t data);
void at24c02_read(uint8_t addr, uint8_t *data);
void at24c02_writePage(uint8_t addr, uint8_t *data, uint8_t n);
void at24c02_readPage(uint8_t addr, uint8_t *data, uint8_t n);

#endif

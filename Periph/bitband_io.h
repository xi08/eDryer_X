// code = utf-8

#ifndef __bitband_io_h
#define __bitband_io_h

#include "stm32f10x.h"
#include <stddef.h>

#define bitBandTran(addr, pos) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (pos << 2))
#define bitBandAddr(addr, pos) (*((volatile size_t *)(bitBandTran(addr, pos))))

#define GPIOA_ODR_Addr (GPIOA_BASE + 12)
#define GPIOA_IDR_Addr (GPIOA_BASE + 8)
#define PAi(pin) bitBandAddr(GPIOA_IDR_Addr, pin)
#define PAo(pin) bitBandAddr(GPIOA_ODR_Addr, pin)

#define GPIOB_ODR_Addr (GPIOB_BASE + 12)
#define GPIOB_IDR_Addr (GPIOB_BASE + 8)
#define PBi(pin) bitBandAddr(GPIOB_IDR_Addr, pin)
#define PBo(pin) bitBandAddr(GPIOB_ODR_Addr, pin)

#define GPIOC_ODR_Addr (GPIOC_BASE + 12)
#define GPIOC_IDR_Addr (GPIOC_BASE + 8)
#define PCi(pin) bitBandAddr(GPIOC_IDR_Addr, pin)
#define PCo(pin) bitBandAddr(GPIOC_ODR_Addr, pin)

#define GPIOD_ODR_Addr (GPIOD_BASE + 12)
#define GPIOD_IDR_Addr (GPIOD_BASE + 8)
#define PDi(pin) bitBandAddr(GPIOD_IDR_Addr, pin)
#define PDo(pin) bitBandAddr(GPIOD_ODR_Addr, pin)

#define GPIOE_ODR_Addr (GPIOE_BASE + 12)
#define GPIOE_IDR_Addr (GPIOE_BASE + 8)
#define PEi(pin) bitBandAddr(GPIOE_IDR_Addr, pin)
#define PEo(pin) bitBandAddr(GPIOE_ODR_Addr, pin)

#define GPIOF_ODR_Addr (GPIOF_BASE + 12)
#define GPIOF_IDR_Addr (GPIOF_BASE + 8)
#define PFi(pin) bitBandAddr(GPIOF_IDR_Addr, pin)
#define PFo(pin) bitBandAddr(GPIOF_ODR_Addr, pin)

#define GPIOG_ODR_Addr (GPIOG_BASE + 12)
#define GPIOG_IDR_Addr (GPIOG_BASE + 8)
#define PGi(pin) bitBandAddr(GPIOG_IDR_Addr, pin)
#define PGo(pin) bitBandAddr(GPIOG_ODR_Addr, pin)

#endif

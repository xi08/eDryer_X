// code = utf-8

#ifndef __bitband_io_h
#define __bitband_io_h

#include "bitband_base.h"

#define PAxIn(pin) bitBandAddr((GPIOA_BASE + 0x08), pin)
#define PAxOut(pin) bitBandAddr((GPIOA_BASE + 0x0c), pin)

#define PBxIn(pin) bitBandAddr((GPIOB_BASE + 0x08), pin)
#define PBxOut(pin) bitBandAddr((GPIOB_BASE + 0x0c), pin)

#define PCxIn(pin) bitBandAddr((GPIOC_BASE + 0x08), pin)
#define PCxOut(pin) bitBandAddr((GPIOC_BASE + 0x0c), pin)

#define PDxIn(pin) bitBandAddr((GPIOD_BASE + 0x08), pin)
#define PDxOut(pin) bitBandAddr((GPIOD_BASE + 0x0c), pin)

#endif

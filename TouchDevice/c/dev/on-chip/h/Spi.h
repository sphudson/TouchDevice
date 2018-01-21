
/*! @file Spi.h
    @brief Spi device driver header file
*/

#ifndef _Spi_H_
#define _Spi_H_

#include "Common.h"

u8_t SpiInit(void);
u8_t SpiWriteByte(u8_t byte);
u8_t SpiReadByte(void);

#endif



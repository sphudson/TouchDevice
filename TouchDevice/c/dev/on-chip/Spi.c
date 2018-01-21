
/*! @file Spi.c
    @brief Spi device driver
*/

#include "Common.h"
#include "System.h"
#include "Spi.h"

/*!
 @brief Initialization function
 */
u8_t SpiInit(void)
{
  /* initialize UCB0 (SPI) interface */
  UCB0CTL1 = UCSWRST;
  UCB0CTL0 = UCMST + UCSYNC + UCMSB;
  UCB0CTL1 |= UCSSEL1;
  UCB0BR0 = 4;
  UCB0CTL1 &= ~UCSWRST;
  IE2 &= ~UCB0RXIE;
  IE2 &= ~UCB0TXIE;
  IFG2 &= ~UCB0RXIFG;  
  IFG2 &= ~UCB0TXIFG;  
  
  return RET_SUCCESS;
}

/*!
 * @brief
 */
u8_t SpiWriteByte(u8_t byte)
{
  UCB0TXBUF = byte;
  while (UCB0STAT & UCBUSY) { };
  return UCB0RXBUF;  
}

/*!
 * @brief
 */
u8_t SpiReadByte(void)
{
  UCB0TXBUF = 0;
  while (UCB0STAT & UCBUSY) { };
  return UCB0RXBUF;
}





/*! @file Uart.c
    @brief Uart device driver
*/

#include "Common.h"
#include "Uart.h"
#include "System.h"

static volatile bool UartIntFlag = FALSE;

/*!
 @brief Initialization function
 */
u8_t UartInit(void)
{  
  /* setup UCA0 (UART) interface */
  UCA0CTL1 = UCSSEL_2;
  UCA0BR0 = 0x82;     // 41=19200@16MHz A0=19200@8MHz 82=9600@16MHz 8a=115200@16MHz
  UCA0BR1 = 0x06;     // 03=19200@16MHz 01=19200@8MHz 06=9600@16MHz 00=115200@16MHz
  UCA0MCTL = UCBRS_6; // 02=19200@16MHz 06=19200@8MHz 06=9600@16MHz 07=115200@16MHz
  UCA0CTL1 &= ~UCSWRST;
  IE2 |= UCA0RXIE;
  IE2 &= ~UCA0TXIE;
  IFG2 &= ~UCA0RXIFG;  
  IFG2 &= ~UCA0TXIFG;  

  return RET_SUCCESS;
}

/*!
 * @brief
 */
void UartWriteByte(u8_t byte)
{
  UCA0TXBUF = byte;
  while (UCA0STAT & UCBUSY) { };
}

/*!
 * @brief
 */
void UartWriteBuf(u8_t *pBuf, u16_t size)
{
  while (size--)
    UartWriteByte(*pBuf++);
}

/*!
 * @brief
 */
u8_t UartReadByte(void)
{
  return UCA0RXBUF;
}

/*!
 * @brief
 */
bool UartIsDataAvail(void)
{
  bool retFlag;
  __disable_interrupt();
  {
    retFlag = UartIntFlag;
  if (retFlag == TRUE)
    UartIntFlag = FALSE;  
  }
  __enable_interrupt();
  return retFlag;
} 

/*!
 * @brief
 */
void UartFlushRx(void)
{
  while (UartIsDataAvail() == TRUE)
    UartReadByte();
}

/*!
 * @brief
 */
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RXInterrupt(void) 
{
  if (IFG2_bit.UCA0RXIFG)
  {
    IFG2_bit.UCA0RXIFG = 0;
    UartIntFlag = TRUE;
  }
  SystemWakeup();
}

/*!
 * @brief
 */
void UartChangeBaudRate(UartBaud_t baud)
{
  IE2 &= UCA0RXIE;
  IE2 &= ~UCA0TXIE;
  IFG2 &= ~UCA0RXIFG;  
  IFG2 &= ~UCA0TXIFG;  

  UCA0CTL1 = UCSSEL_2;

  UCA0CTL1 |= UCSWRST;  
  switch (baud)
  {
  case UART_BAUD_9600:
    UCA0BR0 = 0x82;     // 41=19200@16MHz A0=19200@8MHz 82=9600@16MHz 8a=115200@16MHz
    UCA0BR1 = 0x06;     // 03=19200@16MHz 01=19200@8MHz 06=9600@16MHz 00=115200@16MHz
    UCA0MCTL = UCBRS_6; // 02=19200@16MHz 06=19200@8MHz 06=9600@16MHz 07=115200@16MHz
    break;
    
  case UART_BAUD_115200:
    UCA0BR0 = 0x8a;     // 41=19200@16MHz A0=19200@8MHz 82=9600@16MHz 8a=115200@16MHz
    UCA0BR1 = 0x00;     // 03=19200@16MHz 01=19200@8MHz 06=9600@16MHz 00=115200@16MHz
    UCA0MCTL = UCBRS_7; // 02=19200@16MHz 06=19200@8MHz 06=9600@16MHz 07=115200@16MHz
    break;
  }

  UCA0CTL1 &= ~UCSWRST;

  IE2 |= UCA0RXIE;
  IE2 &= ~UCA0TXIE;
  IFG2 &= ~UCA0RXIFG;  
  IFG2 &= ~UCA0TXIFG;  
}


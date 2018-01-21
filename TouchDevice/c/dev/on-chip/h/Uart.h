
/*! @file Uart.h
    @brief Uart device driver header file
*/

#ifndef _Uart_H_
#define _Uart_H_

#include "Common.h"

typedef enum
{
  UART_BAUD_9600,
  UART_BAUD_115200
} UartBaud_t;

u8_t UartInit(void);
void UartWriteByte(u8_t byte);
void UartWriteBuf(u8_t *pBuf, u16_t size);
u8_t UartReadByte(void);
bool UartIsDataAvail(void);
void UartChangeBaudRate(UartBaud_t baud);
void UartFlushRx(void);

#endif



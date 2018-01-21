
/*! @file Led.h
    @brief LED driver header file
*/

#ifndef _LED_H_
#define _LED_H_

#include "Common.h"

enum
{
  LED_GREEN,
  LED_RED,
  
  LED_NUM_LEDS
};   

u8_t LedInit(void);
u8_t LedOnOff(u8_t ledNum, bool onOff);
u8_t LedToggle(u8_t ledNum);
u8_t LedFlash(u8_t ledNum);

#endif



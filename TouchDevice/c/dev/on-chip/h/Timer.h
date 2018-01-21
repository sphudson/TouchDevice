
/*! @file Timer.h
    @brief Timer driver header file
*/

#ifndef _TIMER_H_
#define _TIMER_H_

#include "common.h"

#define TIMER_PERIOD_MSEC    (20ul)
#define TIMER_FREQ_HZ        (1000ul / TIMER_PERIOD_MSEC)

extern volatile u32_t tickCount;
#define TimerGetTickCount()      (tickCount)

typedef void (*TimerCallbackFnc_t)(void);

u8_t TimerInit(void);
void TimerA0IntrHandler(void);
u8_t TimerRegisterCallbackFnc(TimerCallbackFnc_t fncPtr);

#endif



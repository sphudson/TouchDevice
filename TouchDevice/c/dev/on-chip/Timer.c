
/*! @file Timer.c
    @brief Timer driver module
*/

#include <string.h>

#include "Common.h"
#include "Timer.h"
#include "System.h"
#include "Util.h"

#define TIMER_RELOAD           (SYSTEM_SMCLK_FREQ / TIMER_PERIOD_MSEC / 8ul)
#define TIMER_MAX_CALLBACKS    (4)

static u8_t numCallbacks;
static TimerCallbackFnc_t callbacks[TIMER_MAX_CALLBACKS];

/* system tick counter */
volatile u32_t tickCount = 0;

/*!
 * @brief Timer device module initialization function
 * @retval RET_SUCCESS initialization successful
 */
u8_t TimerInit(void)
{  
  memset(callbacks, 0, sizeof(callbacks));
  
  /* setup Timer A */
  /* up mode, source=SMCLK, /8 */
  TACTL = TASSEL_2 + MC_1 + ID_3;
  TACTL_bit.TAIFG = 0;
  TACTL_bit.TAIE = 0;
  TACCR0 = (u16_t)TIMER_RELOAD;
  TACCTL0_bit.CCIFG = 0;
  TACCTL0_bit.CCIE = 1;

  return RET_SUCCESS;
}

/*!
 * @brief 
 */
u8_t TimerRegisterCallbackFnc(TimerCallbackFnc_t fncPtr)
{
  u8_t retVal = RET_SUCCESS;
  
  if (numCallbacks < TIMER_MAX_CALLBACKS)
  {
    callbacks[numCallbacks++] = fncPtr;
  }
  else
  {
    retVal = RET_FAIL;
  }
  
  return retVal;
}

/*!
 * @brief Timer interrupt handler
 */
#pragma vector = TIMERA0_VECTOR
__interrupt void TimerA0Interrupt(void) 
{   
  u8_t i;
  
  /* re-enable interrupts to allow lower priority interrupts to be
     serviced */
  __enable_interrupt();
  
  /* disable timer interrupts temporarily to eliminate reentrancy due to long
  service calls */
  TACCTL0_bit.CCIE = 0;

  tickCount++;
  
  /* provide callback service to modules */
  for (i = 0; i < numCallbacks; i++)
  {
    if (callbacks[i])
    {
      callbacks[i]();
    }
  }

  /* reenable timer interrupts */
  TACCTL0_bit.CCIE = 1;
  
  SystemWakeup();
}



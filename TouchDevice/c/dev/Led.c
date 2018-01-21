
/*! @file Led.c
    @brief LED driver module
*/

#include <string.h>
#include "Common.h"
#include "Led.h"
#include "Timer.h"
#include "Util.h"

#define LED_GREEN_BIT     (P1OUT_bit.P1OUT_1)
#define LED_RED_BIT       (P1OUT_bit.P1OUT_0)

static s8_t ledFlashCounts[LED_NUM_LEDS];

static void LedTimerCallback(void);

/*!
 * @brief LED device module initialization function
 * @retval RET_SUCCESS initialization successful
 */
u8_t LedInit(void)
{
  TimerRegisterCallbackFnc(LedTimerCallback);
  memset(ledFlashCounts, 0, sizeof(ledFlashCounts));
  
  /* lamp test */
  LedOnOff(LED_GREEN, ON);
  LedOnOff(LED_RED, OFF);
  UtilDelayMsec(500);
  LedOnOff(LED_GREEN, OFF);
  LedOnOff(LED_RED, ON);
  UtilDelayMsec(500);
  
  /* initial state all LEDs off */
  LedOnOff(LED_GREEN, OFF);
  LedOnOff(LED_RED, OFF);

  return RET_SUCCESS;
}

/*!
 * @brief Turn a particular LED on or off
 * @param[in] ledNum LED number (see led.h)
 * @param[in] onOff TRUE=on, FALSE=off
 * @retval RET_SUCCESS successful operation
 */
u8_t LedOnOff(u8_t ledNum, bool onOff)
{
  u8_t retVal = RET_SUCCESS;
  
  switch (ledNum)
  {
  case LED_GREEN:    LED_GREEN_BIT = onOff;   break;
  case LED_RED:      LED_RED_BIT   = onOff;   break;
  
  default:
    retVal = RET_FAIL;
    break;
  }

  return retVal;
}

/*!
 * @brief Toggle a particular LED on or off
 * @param[in] ledNum LED number (see led.h)
 * @retval RET_SUCCESS successful operation
 */
u8_t LedToggle(u8_t ledNum)
{
  u8_t retVal = RET_SUCCESS;

  switch (ledNum)
  {
  case LED_GREEN:    LED_GREEN_BIT    ^= 1;  break;
  case LED_RED:      LED_RED_BIT      ^= 1;  break;

  default:
    retVal = RET_FAIL;
    break;
  }

  return retVal;
}

/*!
 * @brief Flash a particular LED on and then off
 * @param[in] ledNum LED number (see led.h)
 * @retval RET_SUCCESS successful operation
 * @note This function leaves the LED on for a hard-coded period of time
 */
u8_t LedFlash(u8_t ledNum)
{
  ledFlashCounts[ledNum] = 2;
  LedOnOff(ledNum, ON);
  
  return RET_SUCCESS;
}

/*!
 * @brief LED timer service function invoked by the timer ISR.
 * @note Processing in this function should be kept to an absolute minimum.
 */
static void LedTimerCallback(void)
{ 
  u8_t i;
  
  for (i = 0; i < LED_NUM_LEDS; i++)
  {
    if (ledFlashCounts[i])
    {
      ledFlashCounts[i]--;
      if (ledFlashCounts[i] <= 0)
        LedOnOff(i, OFF);
    }
  }
}

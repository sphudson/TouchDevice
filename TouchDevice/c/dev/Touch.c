
/*! @file Touch.c
    @brief Touch pad driver module
*/

#include <string.h>
#include "Common.h"
#include "Touch.h"
#include "Util.h"
#include "Pinnacle.h"
#include "Led.h"

static TouchCallbackFnc_t callbackFncPtr;

static void TouchCallback(u8_t *dataBuf, u8_t numBytes);

/*!
 * @brief Touch device module initialization function
 * @retval RET_SUCCESS initialization successful
 */
u8_t TouchInit(void)
{
  u8_t retVal = RET_SUCCESS;
  
  retVal |= PinnacleInit();
  retVal |= PinnacleRegisterCallbackFnc(TouchCallback);
  
  return retVal;
}

/*!
 * @brief 
 */
u8_t TouchRegisterCallbackFnc(TouchCallbackFnc_t fncPtr)
{
  callbackFncPtr = fncPtr;
  
  return RET_SUCCESS;
}

/*!
 * @brief 
 */
static void TouchCallback(u8_t *dataBuf, u8_t numBytes)
{
  /* provide callback service */
  if (callbackFncPtr)
  {
    u16_t x, y, z;
    
    /* absolute data from Pinnacle */
    x = ((u16_t)(dataBuf[3] & 0x0F) << 8) | dataBuf[1];
    y = ((u16_t)(dataBuf[3] & 0xF0) << 4) | dataBuf[2];
    z = dataBuf[4];
    
    callbackFncPtr(x, y, z);
  }
}


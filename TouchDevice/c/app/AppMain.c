
/*! @file AppMain.c
    @brief Application main module
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "App.h"
#include "Common.h"
#include "Util.h"
#include "Led.h"
#include "Button.h"
#include "Uart.h"
#include "Touch.h"
#include "Timer.h"

#define REPORTING_PERIOD      (50)  /* msec */
#define REPORTING_TIMEOUT     (REPORTING_PERIOD / TIMER_PERIOD_MSEC)

/* wheel constants */
#define WHEEL_ANGLE_THRESHOLD (18)
#define CIRCLE_SIZE           (0.35) /* 0.00-1.00 of touchpad surface */
#define CIRCLE_RADIUS         ((TOUCH_MAX_Y / 2) * (TOUCH_MAX_Y / 2) * CIRCLE_SIZE)

/* swipe constants */
#define MAX_SWIPE_PERIOD      (500 / TIMER_PERIOD_MSEC)
#define MIN_SWIPE_PERIOD      (20 / TIMER_PERIOD_MSEC)
#define MIN_SWIPE_SIZE        (0.03) /* 0.00-1.00 of touchpad surface */
#define MIN_SWIPE_LENGTH      (TOUCH_MAX_Y * TOUCH_MAX_Y * MIN_SWIPE_SIZE)
#define MIN_SWIPE_UP_ANGLE    (155)
#define MAX_SWIPE_UP_ANGLE    (205)
#define MIN_SWIPE_DOWN_ANGLE  (335)
#define MAX_SWIPE_DOWN_ANGLE  (25)
#define MIN_SWIPE_LEFT_ANGLE  (245)
#define MAX_SWIPE_LEFT_ANGLE  (295)
#define MIN_SWIPE_RIGHT_ANGLE (65)
#define MAX_SWIPE_RIGHT_ANGLE (115)
              
typedef enum
{
  MODE_XY_CURSOR,
  MODE_WHEEL
} MODE_t;

typedef enum
{
  SWIPE_NONE,
  SWIPE_UP,
  SWIPE_DOWN,
  SWIPE_LEFT,
  SWIPE_RIGHT
} SWIPE_DIRECTION_t;

static void AppTouchCallback(u16_t _x, u16_t _y, u16_t _z);
static void AppTimerCallback(void);

static volatile bool reportTimerFlag = FALSE;
static volatile bool dataAvailFlag = FALSE;
static s16_t x, y, z;

/*!
 * @brief Application initialization function.
 * @note This function should only be called once at runtime.
 */
u8_t AppInit(void)
{ 
  u8_t retVal = RET_SUCCESS;
  
  retVal |= TouchRegisterCallbackFnc(AppTouchCallback);
  retVal |= TimerRegisterCallbackFnc(AppTimerCallback);
  
  return retVal;
}

/*!
 * @brief Application main entry point.
 */
void AppMain(void)
{  
  static char buf[48];
  s16_t prevX=0, prevY=0, prevZ=0, relX=0, relY=0;
  s16_t absX=0, absY=0, prevAbsX=0, prevAbsY=0;
  s16_t swipeX=0, swipeY=0, swipeAngle=0, angle=0;
  s16_t startAngle=0, wheelAngle=0;
  SWIPE_DIRECTION_t swipeDirection = SWIPE_NONE;
  u16_t swipeStartTime=0;
  u32_t swipePeriod=0;
  bool hostReady = FALSE;
  MODE_t mode = MODE_XY_CURSOR;
  
  LedFlash(LED_GREEN);
  
  for (;;)
  {   
    if (UartIsDataAvail() == TRUE)
    {
      if (UartReadByte() == 'a')
        hostReady = TRUE;
    }
    
    if (dataAvailFlag)
    {
      LedFlash(LED_GREEN);
          
      dataAvailFlag = FALSE;
      
      /* if finger pressed on touch pad */
      if (z)
      {
        bool outsideCircle = FALSE;

        /* remember absolute X and Y from touchpad */
        absX = x;
        absY = y;
        
        /* XY origin is at center of touchpad */
        x = x - (TOUCH_MAX_X / 2);
        y = y - (TOUCH_MAX_Y / 2);
        
        /* determine if XY point outside circle */
        outsideCircle = ((u32_t)((s32_t)x * (s32_t)x) +
                         (u32_t)((s32_t)y * (s32_t)y) -
                         CIRCLE_RADIUS) <= 0 ? FALSE: TRUE;
        
        /* if no finger on touchpad previously */
        if (prevZ == 0)
        {
          /* possible swipe - mark new starting XY location, timestamp */
          swipeStartTime = tickCount;
          swipeX = x;
          swipeY = y;
          prevX = x;
          prevY = y;
  
          /* determine tracking mode mode */
          if (outsideCircle)
            mode = MODE_WHEEL;
          else
            mode = MODE_XY_CURSOR;
        }
       
        if (mode == MODE_WHEEL)
        {
          swipePeriod = tickCount - swipeStartTime;
          
          /* compute wheel angle 0-359 degrees */
          angle = (atan2(x, y) * (180 / __PI));
          
          if (angle < 0)
            angle = 360 + angle;
          
          if (prevZ == 0)
          {
            startAngle = angle;
          }
            
          if (swipePeriod > MAX_SWIPE_PERIOD)
          {
            wheelAngle = (angle - startAngle);
            if (UtilAbs(wheelAngle) > 180)
            {
              if (wheelAngle < 0)
                wheelAngle = 360 + wheelAngle;
              else
                wheelAngle = wheelAngle - 360;
            }
  
            if (UtilAbs(wheelAngle) > WHEEL_ANGLE_THRESHOLD)
            {
              startAngle = angle;
            }
            else
              wheelAngle = 0;
          }
        }
        
        if (mode == MODE_XY_CURSOR)
        {
          /* compute relative XY differences */
          relX = x - prevX;
          relY = y - prevY;
        } 
      }
      else
      {
        if (prevZ)
        {
          swipePeriod = tickCount - swipeStartTime;
          swipeDirection = SWIPE_NONE;
          
          if ((swipePeriod >= MIN_SWIPE_PERIOD) &&
              (swipePeriod <= MAX_SWIPE_PERIOD))
          {
            swipeX = prevX - swipeX;
            swipeY = prevY - swipeY;
            swipeStartTime = 0;
            
            /* check to see if swipe vector magnitude >MIN length */
            if ((u32_t)((s32_t)swipeX * (s32_t)swipeX) + 
                (u32_t)((s32_t)swipeY * (s32_t)swipeY) >= MIN_SWIPE_LENGTH)
            {
              /* compute swipe angle 0-359 degrees */
              swipeAngle = (atan2(swipeX, swipeY) * (180 / __PI));
              
              if (swipeAngle < 0)
                swipeAngle = 360 + swipeAngle;
              
              /* determine discrete swipe direction */
              if ((swipeAngle > MIN_SWIPE_DOWN_ANGLE) || 
                  (swipeAngle < MAX_SWIPE_DOWN_ANGLE))
                swipeDirection = SWIPE_DOWN;
              else
                if ((swipeAngle > MIN_SWIPE_RIGHT_ANGLE) && 
                    (swipeAngle < MAX_SWIPE_RIGHT_ANGLE))
                  swipeDirection = SWIPE_RIGHT;
              else
                if ((swipeAngle > MIN_SWIPE_UP_ANGLE) && 
                    (swipeAngle < MAX_SWIPE_UP_ANGLE))
                  swipeDirection = SWIPE_UP;
              else
                if ((swipeAngle > MIN_SWIPE_LEFT_ANGLE) && 
                    (swipeAngle < MAX_SWIPE_LEFT_ANGLE))
                  swipeDirection = SWIPE_LEFT;
              
              if (swipeDirection != SWIPE_NONE)
                LedFlash(LED_RED);
            }
          }
        }
      }
      
      prevX = x;
      prevY = y;
      prevZ = z;
      prevAbsX = absX;
      prevAbsY = absY;
    }
      
    if (hostReady && (relX || relY || wheelAngle || swipeDirection || 
                      (absX != prevAbsX) || (absY != prevAbsY)))
    {
      sprintf(buf, "%04x%04x%04x%04x%04x%04x\r", 
              relX, relY, wheelAngle, swipeDirection, absX, absY);
      UartWriteBuf((u8_t *)buf, strlen(buf));
      hostReady = FALSE;
      relX = 0;
      relY = 0;
      wheelAngle = 0;
      swipeDirection = SWIPE_NONE;
    }
  }
}

/*
  Compute 
*/

/*!
 * @brief 
 */
static void AppTouchCallback(u16_t _x, u16_t _y, u16_t _z)
{  
  if (dataAvailFlag == FALSE)
  {
    x = _x;
    y = _y;
    z = _z;
    dataAvailFlag = TRUE;
  }
}

/*!
 * @brief 
 */
static void AppTimerCallback(void)
{
  static u8_t timeout = REPORTING_TIMEOUT;
  
  if (timeout)
  {
    timeout--;
  }
  
  if (timeout == 0)
  {
    timeout = REPORTING_TIMEOUT;
    reportTimerFlag = TRUE;
  }
}

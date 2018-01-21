
/*! \file Touch.h
    \brief Touch pad driver module header file
*/

#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "Common.h" 

#define TOUCH_MAX_X       (1920ul)
#define TOUCH_MAX_Y       (1408ul)

typedef void (*TouchCallbackFnc_t)(u16_t x, u16_t y, u16_t z);

u8_t TouchInit(void);
u8_t TouchRegisterCallbackFnc(TouchCallbackFnc_t fncPtr);

#endif




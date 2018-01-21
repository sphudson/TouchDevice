
/*! \file Pinnacle.h
    \brief Cirque Pinnacle ASIC low-level device driver header file
*/

#ifndef _PINNACLE_H_
#define _PINNACLE_H_

#include "Common.h" 

typedef void (*PinnacleCallbackFnc_t)(u8_t *dataBuf, u8_t numBytes);

u8_t PinnacleInit(void);
u8_t PinnacleRegisterCallbackFnc(PinnacleCallbackFnc_t fncPtr);

#endif




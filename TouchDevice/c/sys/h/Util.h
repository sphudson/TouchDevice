
/*! @file Util.h
    @brief Utility header file
*/

#ifndef _UTILS_H_
#define _UTILS_H_

#include "Common.h"
#include "System.h"

#define UtilDelaySec(x)        __delay_cycles((x)*(SYSTEM_MCLK_CLOCK))
#define UtilDelayMsec(x)       __delay_cycles((x)*(SYSTEM_MCLK_CLOCK/1000))
#define UtilDelayUsec(x)       __delay_cycles((x)*(SYSTEM_MCLK_CLOCK/1000000))
#define UtilAbs(x)             (((x) < 0) ? -(x): (x))
#define UtilMax(a, b)          (((a) > (b)) ? (a) : (b))
#define UtilMin(a, b)          (((a) < (b)) ? (a) : (b))

#define UTIL_SCRATCH_BUF_SIZE  (192)

u8_t UtilInit(void);
u16_t UtilChecksum(u8_t *pBuf, u16_t numBytes);
u8_t *UtilGetScratchBufPtr(void);

#endif


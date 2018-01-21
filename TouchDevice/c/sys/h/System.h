
/*! @file System.h
    @brief System header file
*/

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "Common.h"

#define SYSTEM_MCLK_CLOCK    (16000000ul)  /**< CPU clock frequency (in Hz) */
#define SYSTEM_ACLK_FREQ     (12000ul)     /**< ACLK clock frequency (in Hz) */
#define SYSTEM_SMCLK_FREQ    (SYSTEM_MCLK_CLOCK)  /**< SMCLK clock frequency (in Hz) */

#define SystemSleep()       // (LPM3)
#define SystemWakeup()      // (LPM3_EXIT)

u8_t SystemInit(void);
void SystemReset(void);

#endif 

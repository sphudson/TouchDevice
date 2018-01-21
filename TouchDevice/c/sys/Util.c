
/*! @file Util.c
    @brief Utility module
*/

#include "Common.h"
#include "Util.h"

static u8_t scratchBuf[UTIL_SCRATCH_BUF_SIZE];

/*!
 @brief Initialization function
 */
u8_t UtilInit(void)
{
  return RET_SUCCESS;
}

/*!
 * @brief
 */
u16_t UtilChecksum(u8_t *pBuf, u16_t numBytes)
{
  u16_t checkSum = 0;
  
  while (numBytes--)
    checkSum += *pBuf++;
  
  return checkSum;
}

/*!
 * @brief
 */
u8_t *UtilGetScratchBufPtr(void)
{
  return scratchBuf;
}



/*! @file Button.c
    @brief Button driver module
*/

#include <string.h>
#include "Common.h"
#include "Button.h"
#include "Util.h"

#define BUTTON_BIT      (P1IN_bit.P1IN_2)

/*!
 * @brief Button device module initialization function
 * @retval RET_SUCCESS initialization successful
 */
u8_t ButtonInit(void)
{
  return RET_SUCCESS;
}

/*!
 * @brief
 */
u8_t ButtonIsPressed(void)
{
  return BUTTON_BIT ? FALSE: TRUE;
}



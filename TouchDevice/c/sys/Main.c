
/*! @file Main.c
    @brief Main module
*/

#include "Common.h"
#include "Main.h"
#include "System.h"
#include "App.h"

/*!
 * @brief C-language main() function.  Invoked once at runtime.
 */
void main(void)
{    
  /* initialize system, peripherals, and subsystems */
  SystemInit();
  
  /* jump to main application entry point */
  AppMain();
}


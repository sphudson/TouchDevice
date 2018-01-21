
/*! @file System.c
    @brief System module
*/

#include "Common.h"
#include "System.h"
#include "Uart.h"
#include "App.h"
#include "Util.h"
#include "Timer.h"
#include "Led.h"
#include "Button.h"
#include "Spi.h"
#include "Touch.h"

/*!
 * @brief System initialization function
 * @retval RET_SUCCESS initialization successful
 * @warning This function should only be called once at runtime.
 */
u8_t SystemInit(void)
{   
  u8_t retVal = RET_SUCCESS;
  
  /* stop watchdog */
  WDTCTL = WDTPW + WDTHOLD;

  /* delay for ~1sec (clock is ~1MHz at power-up) for Vcc to stabilize */
  __delay_cycles(1000000);

  /* DCO to maximum */
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL = CALDCO_16MHZ;
  
  /* setup for VLOCLK mode for ACLK */
  BCSCTL1_bit.XTS = 0;
  BCSCTL3_bit.LFXT1S1 = 1;
  BCSCTL3_bit.LFXT1S0 = 0;
    
  __disable_interrupt();
  
  /* I/O port direction: 0=input, 1=output 
     Note: set all unused I/O pins as outputs to save power
  */

  /* setup port 1 */
  P1SEL = 0;
  P1OUT = BIT2;
  P1DIR = BIT0 + BIT1;
  P1REN = BIT2;
  P1IE = 0;
  P1IES = 0;

  /* setup port 2 */
  P2SEL = 0;
  P2OUT = 0;
  P2DIR = 0;
  P2IE = BIT0;
  P2IFG = 0;
  P2IES = 0;  //BIT0;

  /* setup port 3 */
  P3SEL = BIT1 + BIT2 + BIT3 + BIT4 + BIT5;
  P3OUT = BIT0;
  P3DIR = BIT0 + BIT1 + BIT3 + BIT4;

  /* setup port 4 */
  P4SEL = 0;
  P4OUT = 0;
  P4DIR = 0;
  P4REN = 0;

  __enable_interrupt();    
  
  /* initialize baseline suport subsystems and device drivers */
  retVal |= UtilInit();
  retVal |= TimerInit();
  retVal |= UartInit();
  retVal |= SpiInit();
  retVal |= LedInit();
  retVal |= ButtonInit();
  retVal |= TouchInit();
  
  /* application init comes last after all peripherals/modules up */
  retVal |= AppInit();

  return retVal;
}

/*!
 * @brief Low-level init function called by cstartup before data is initialized.
 * @return Always returns a value of 1
 */
/*!
 \brief Low-level init function called by cstartup before data is initialized.
 */
int __low_level_init(void)
{
  /* stop watchdog */
  WDTCTL = WDTPW + WDTHOLD;

  return 1;
}

void SystemReset(void)
{
  WDTCTL = 0;
  while (1) { };
}

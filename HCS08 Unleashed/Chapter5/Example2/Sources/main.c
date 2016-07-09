// LED flasher and reset testing
#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1  PTBD_PTBD6  // LED1 is connected to PTB6
#define LED2  PTBD_PTBD7  // LED2 is connected to PTB7
#define SW1   PTAD_PTAD2  // SW1 is connected to PTA2
#define CLRCOP {SRS = 0;} // Clear Watchdog Macro

void main(void) 
{
  unsigned int temp;
  // configure SOPT1 register, enable COP (BKGD pin is disabled)
  SOPT1 = bCOPT | bCOPE;  
  PTBDD = BIT_7 | BIT_6;  	// configure pins 6 and 7 of port B as outputs
  PTAPE = BIT_2;	          // enable internal pull up on pin PTA2
  // Now we test SRS to detect if the reset source was a COP timeout
  // if SRS_COP = 1 then we turn LED2 on, if COP_COP = 0 then we turn LED2 off
  if (SRS_COP) LED2 = 0; else LED2 = 1;
  while (1)
  {
    PTBD_PTBD6 = 1;		                      // LED = off
    for (temp=30000; temp; temp--) CLRCOP;	// wait for a while
    PTBD_PTBD6 = 0;		                      // LED = on
    for (temp=3000; temp; temp--) CLRCOP;	  // wait for a while
    while (!SW1);		                        // while SW1 is pressed, stay here
  }
}

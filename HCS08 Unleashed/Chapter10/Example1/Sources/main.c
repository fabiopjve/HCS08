// DEMO9S08QG8 - ACMP example (no interrupt)
// LED1 connected to PTB6, LED2 connected to PTB7
// RV1 connected to PTA0 (ACMP+), RZ connected to PTA1 (ACMP-)

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1_ON	PTBD = BIT_6
#define LED2_ON	PTBD = BIT_7

void main(void)
{
  SOPT1 = bBKGDPE;  		    // Enable debug pin
  PTBDD = BIT_7 | BIT_6;  	// PTB6 and PTB7 as outputs
  ACMPSC = bACME;   		    // Enable ACMP
  while(1)
  {
    if (ACMPSC_ACO)
    {               		// if ACO=1 (ACMP+>ACMP-)
      LED1_ON;
    } else
    {               		// if ACO=0 (ACMP+<ACMP-)
      LED2_ON;
    }
  }
}





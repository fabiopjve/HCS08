// Spyder08 LED flasher
#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

void main(void) 
{
  unsigned int temp;
  SOPT1 = bBKGDPE;  // configure SOPT1 register, enable pin BKGD for BDM
  PTADD = BIT_4 | BIT_3 | BIT_2 | BIT_1 | BIT_0; // bit 4 to bit 0 as outputs
  while (1)
  {
    PTAD |= BIT_0;	                // LED = on
    for (temp=10000; temp; temp--);	// wait for a while
    PTAD &= ~BIT_0;	                // LED = off
    for (temp=10000; temp; temp--);	// wait for a while
  }
}




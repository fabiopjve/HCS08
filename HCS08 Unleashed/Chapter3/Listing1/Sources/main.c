// A simple LED flasher
#include <hidef.h>        /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  	    // This is our definition file !

void main(void) 
{
  unsigned int temp;
  unsigned char counter1;
  SOPT1 = bBKGDPE;  // configure SOPT1 register, enable pin BKGD for BDM
  PTBDD = BIT_6;    // configure pin 6 of port B as an output
  counter1 = 0;
  while (1)
  {
    PTBD_PTBD6 = 1;                 	// LED = off
    for (temp=10000; temp; temp--); 	// wait for a while
    PTBD_PTBD6 = 0;	                  // LED = on
    for (temp=10000; temp; temp--); 	// wait for a while
    counter1++;
  }
}


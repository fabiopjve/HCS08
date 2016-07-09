// Another LED flasher
#include <hidef.h> 		/* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		// This is our definition file !

#pragma profile on

unsigned char counter1;
unsigned int val;

void delay(unsigned int value)
{
  for (;value; value--);
}

void main(void) 
{
  SOPT1 = bBKGDPE;  // configure SOPT1 register, enable pin BKGD for BDM
  PTBDD = BIT_6;    // configure pin 6 of port B as an output
  counter1 = 0;
  val = 10000;
  while (1)
  {
    PTBD_PTBD6 = 1;	// LED = off
    delay(val); 	// wait for a while
    PTBD_PTBD6 = 0;	// LED = on
    delay(val); 	// wait for a while
    counter1++;
    if (counter1>20) val = 30000;
  }
}



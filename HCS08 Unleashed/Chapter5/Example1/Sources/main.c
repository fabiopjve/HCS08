// DEMO9S08QG8 - LED flasher and reset testing
#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1  PTBD_PTBD6  	// LED1 is connected to PTB6
#define LED2  PTBD_PTBD7  	// LED2 is connected to PTB7

void delay(unsigned int value)
{
  for (;value; value--);
}

void reset(void)
{
  unsigned int temp = 0x9E00;
  __asm
  {
    LDHX  @temp			// load the address of temp into H:X
    JMP   ,X			  // jump to the address pointer to by H:X
  }
}

void main(void) 
{
  unsigned char counter1 = 0;
  SOPT1 = bBKGDPE;  		    // configure SOPT1 register, enable pin BKGD for BDM
  PTBDD = BIT_7 | BIT_6;  	// configure pins 6 and 7 of port B as outputs
  // Now we test SRS to detect if the reset source was an ILOP
  // if ILOP = 1 then we turn LED2 on, if ILOP = 0 then we turn LED2 off
  if (SRS_ILOP) LED2 = 0; else LED2 = 1;
  while (1)
  {
    PTBD_PTBD6 = 1;		// LED = off
    delay(30000);		  // wait for a while
    PTBD_PTBD6 = 0;		// LED = on
    delay(3000);		  // wait for a while
    counter1++;
    if (counter1>=25) reset();
  }
}



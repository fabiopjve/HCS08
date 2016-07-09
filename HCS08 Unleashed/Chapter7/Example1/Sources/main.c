// DEMO9S08QG8 ICS demo (16 MHz)
#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

void main(void) 
{
  unsigned int temp;
  SOPT1 = bBKGDPE;	        // configure SOPT1 register, enable pin BKGD for BDM
  ICSSC = NV_FTRIM;	        // configure FTRIM value
  ICSTRM = NV_ICSTRM;	      // configure TRIM value
  ICSC1 = ICS_FLL | bIREFS;	// select FEI mode (ICSOUT = DCOOUT = 512 * IRCLK)
  ICSC2 = BDIV_1;         	// ICSOUT = DCOOUT / 1
  PTBDD = 0xFF;		          // PTB0 through PTB7 as outputs
  PTBD = 0xC0;		          // turn off LED1 and LED2, clear other PTB outputs
  while (1)
  {
    PTBD_PTBD6 = 0;			            // LED = on
    for (temp=3000; temp; temp--);	// wait for a while
    PTBD_PTBD6 = 1;			            // LED = off
    for (temp=30000; temp; temp--);	// wait for a while
  }
}



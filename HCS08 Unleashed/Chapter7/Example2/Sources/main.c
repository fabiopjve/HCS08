// DEMO9S08QG8 ICS example (FBI mode, BUSCLK = 15.625 kHz)
#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

void main(void) 
{
  unsigned int temp;
  SOPT1 = bBKGDPE;		    // configure SOPT1 register, enable pin BKGD for BDM
  ICSSC = NV_FTRIM;		    // configure FTRIM value
  ICSTRM = NV_ICSTRM;	    // configure TRIM value
  ICSC1 = ICS_FBI;  		  // select FBI mode (ICSOUT = IRCLK)
  ICSC2 = BDIV_1 | bICS_LP; 	// ICSOUT = IRCLK / 1, FLL is disabled
  PTBDD = 0xFF;			      // PTB0 through PTB7 as outputs
  PTBD = 0xC0;		      	// turn off LED1 and LED2, clear other PTB outputs
  while (1)
  {
    PTBD_PTBD6 = 0;			          // LED = on
    for (temp=6; temp; temp--);	  // wait for a while
    PTBD_PTBD6 = 1;			          // LED = off
    for (temp=59; temp; temp--);	// wait for a while
  }
}


// DEMO9S08QG8 RTI in stop1 mode example
// LED1 cathode is connected to PTB6

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1  	PTBD_PTBD6

// A simple software loop delay
void delay(unsigned int value)
{
  for (;value; value--);
}


void main(void) 
{
  // configure SOPT1 register, enable STOP instruction and BKGD pin
  SOPT1 = bSTOPE | bBKGDPE;
  SPMSC1 = 0;		        // disable LVD
  SPMSC2 = bPDC;	      // stop1 mode selected
  PTBDD = 0xFF;		      // PTB pins as outputs
  PTBD = 0xFF;		      // Set all PTB outputs (turn leds off)
  SRTISC = RTI_512ms;	  // enable RTI (internal clock source, 512ms timeout)
  if (SPMSC2_PDF)	      // if PDF is set
  {
    SPMSC2_PPDACK = 1;	// acknowledge PDF
  }
  while (1)
  {
    LED1 = 0;			      // LED1 is on
    delay(20000);		    // delay
    LED1 = 1;			      // LED1 is off
    STOP;			          // enter stop1 mode
    // the program does not continue past the STOP instruction above!
  }
}



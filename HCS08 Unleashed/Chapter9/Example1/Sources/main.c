// DEMO9S08QG8 RTI in stop3 mode example
// LED1 cathode is connected to PTB6

#include <hidef.h> 	      /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  	    // This is our definition file!

#define LED1  	PTBD_PTBD6

// A simple software loop delay
void delay(unsigned int value)
{
  for (;value; value--);
}

// ISR for RTI interrupt (only clears RTIF flag)
void interrupt VectorNumber_Vrti rti_isr(void)
{
  SRTISC_RTIACK = 1;	// acknowledge RTI interrupt (clear RTIF)
}

void main(void) 
{
  // configure SOPT1 register, enable STOP instruction and BKGD pin
  SOPT1 = bSTOPE | bBKGDPE;
  SPMSC1 = 0;		    // disable LVD
  SPMSC2 = 0;		    // stop3 mode selected
  PTBDD = 0xFF;	    // PTB pins as outputs
  PTBD = 0xFF;	    // Set all PTB outputs 
  // enable RTI (internal clock source, 512ms timeout, interrupt enabled)
  SRTISC = bRTIE | RTI_512ms;	
  EnableInterrupts;	// enable interrupts
  while (1)
  {
    LED1 = 0;			  // LED1 is on
    delay(20000);		// delay
    LED1 = 1;			  // LED1 is off
    STOP;			      // enter stop1 mode
  }
}




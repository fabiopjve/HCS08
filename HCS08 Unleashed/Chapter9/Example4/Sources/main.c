// DEMO9S08QG8 Real-time clock example using the RTI module

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

unsigned char seconds, minutes, hours;

// RTI ISR (implements a real-time clock)
void interrupt VectorNumber_Vrti rti_isr(void)
{
  SRTISC_RTIACK = 1;		  // acknowledge RTI interrupt (clear RTIF)
  seconds++;			        // increment seconds
  if (seconds>59)		      // do we have more than 59 seconds ?	
  {
    seconds = 0;		      // yes, so we force seconds to zero ...
    minutes++;			      // and increment minutes
    if (minutes>59)		    // do we have more than 59 minutes ?
    {
      minutes = 0;		    // yes, so we force minutes to zero ...
      hours++;			      // and increment hours
      if (hours>23) hours = 0;
    }
  }
}

void main(void) 
{
  // configure SOPT1 register, enable STOP instruction and BKGD pin
  SOPT1 = bSTOPE | bBKGDPE;
  // Configure ICS: external crystal, ICSERCLK enabled in stop mode
  // ICSOUT = 32768 * 512 = 16.7 MHz, BUSCLK = 8.4 MHz
  // IRCLK = disabled, ERCLK = 32768 Hz
  ICSC2 = bEREFS | bERCLKEN | bEREFSTEN;
  // Configure ICS: FEE mode, external reference, RDIV = divide by 1
  ICSC1 = ICS_FLL;  	    // ICS in FEE mode (FLL engaged external because IREFS = 0)
  while (ICSSC_CLKST != ICSC1_CLKS);	// wait for the clock change to take effect
  SPMSC1 = 0;		          // disable LVD
  SPMSC2 = 0;		          // stop3 mode selected
  PTBDD = 0xFF;		        // PTB pins as outputs 
  PTBD = 0xFF;		        // Set all PTB outputs
  // Configure RTI: interrupt enabled, external clock, divide by 32768 
  SRTISC = bRTIE | RTI_DIV32768;
  EnableInterrupts;	      // enable interrupts (CCR:I = 0)
  while (1)
  {
    //STOP;
  }
}

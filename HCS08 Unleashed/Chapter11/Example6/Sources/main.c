// DEMO9S08QG8 - SCI TX example 1
#include <hidef.h>        	/* for EnableInterrupts macro */
#include "derivative.h"   	/* include peripheral declarations */
#include "hcs08.h"        	// This is our header file!

void main(void)
{
  SOPT1 = bBKGDPE;  		// enable the debug pin
  // Following a reset, BUSCLK = 4MHz 
  SCIBD = 26;       		// SCI baud rate = 4MHz/(16*26)=9615 bps
  SCIC2 = bTE;      		// enable the transmitter section
  SCID = 'H';       		// send 'H' through the SCI
  while(1);
}

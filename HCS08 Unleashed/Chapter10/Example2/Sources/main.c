// DEMO9S08QG8 - ACMP example (using interrupt)
// LED1 connected to PTB6
// LED2 connected to PTB7
// RV1 connected to PTA0 (ACMP+)
// RZ connected to PTA1 (ACMP-)

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1  PTBD_PTBD6
#define LED2  PTBD_PTBD7

unsigned int counter;

void interrupt VectorNumber_Vacmp acmp_isr(void)
{
  ACMPSC_ACF = 1; 		// clear interrupt flag
  counter++; 
}

void main(void)
{
  SOPT1 = bBKGDPE;        	// Enable debug pin
  PTBD = BIT_7 | BIT_6;   	// turn leds off
  PTBDD = BIT_7 | BIT_6;  	// PTB6 and PTB7 as outputs
  // Enable ACMP, ACMP interrupts, ACF is set on falling edges of ACO
  ACMPSC = bACME | bACIE | ACMOD_0;   
  counter = 0;
  EnableInterrupts;
  while(1)
  {
    if (counter>10) LED1 = 0; // turn LED1 on when counter>10
    if (counter>20) LED2 = 0; // turn LED2 on when counter>20
  }
}

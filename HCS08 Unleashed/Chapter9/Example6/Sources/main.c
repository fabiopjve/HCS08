// DEMO9S08QG8 MTIM example
// LED1 cathode is connected to PTB6

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1  PTBD_PTBD6

unsigned int timer1;

// MTIM ISR (one interrupt at every millisecond)
void interrupt VectorNumber_Vmtim mtim_isr(void)
{
  MTIMSC_TOF = 0;       	// clear TOF
  if (timer1) timer1--; 	// if timer1 is greater than zero, decrement it
  // insert other software timers here...
}

void main(void) 
{
  // configure SOPT1 register, enable the BKGD pin
  SOPT1 = bBKGDPE;
  // ICS module is operating at 8MHz after reset (BUSCLK = 4MHz)
  PTBDD = 0xFF;       	// PTB pins as outputs 
  PTBD = 0xFF;        	// Set all PTB outputs
  // Configure MTIM: interrupt enabled, MTIM clock = BUSCLK/32, modulo = 125
  // Ttof = 4MHz/(32*125) = 1000
  MTIMSC = bTOIE;
  MTIMCLK = MTIM_DIV32;
  MTIMMOD = 124;      	// MTIM modulo = 124+1 = 125
  EnableInterrupts;   	// Enable interrupts
  while (1)
  {
    LED1 = 0;         	// turn LED1 on
    timer1 = 100;     	// initialize timer1 (100ms)
    while (timer1);   	// wait for timer1 timeout
    LED1 = 1;         	// turn LED1 off
    timer1 = 200;     	// initialize timer1 (200ms)
    while (timer1);   	// wait for timer1 timeout
  }
}

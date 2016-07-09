// DEMO9S08QG8 IRQ-controlled Led flasher
// LED1 cathode is connected to PTB6
// RESET key is connected to PTA5
#include <hidef.h> 		      /* for EnableInterrupts macro */
#include "derivative.h" 		/* include peripheral declarations */
#include "hcs08.h"  		    // This is our definition file!

unsigned char blinking=0;

// This is the IRQ interrupt servicing routine
void interrupt VectorNumber_Virq IRQ_isr()
{
  IRQSC_IRQACK = 1; 	  // acknowledge IRQ interrupt (clear IRQF)
  blinking = !blinking;	// change blinking state
  // IRQ interrupt is disabled to avoid bouncing noise:
  IRQSC_IRQIE = 0;		  // disable IRQ interrupt
}

void main(void) 
{
  unsigned int temp;
  SOPT1 = bBKGDPE;		// configure SOPT1 register, enable pin BKGD for BDM
  PTADD = 0;			    // all PTA pins as inputs
  PTBDD = 0xFE;		    // pin PTB0 as input, PTB1 through PTB7 as outputs
  PTBD = 0xC0;		    // turn off LED1 and LED2, clear other PTB outputs
  PTAPE = BIT_5;		  // PTA5 pull up enabled
  IRQSC = bIRQPE | bIRQIE;	// IRQ pin enabled, IRQ interrupt enabled
  EnableInterrupts;	  // enable interrupts (CCR:I = 0)
  while (1)
  {
    if (blinking)
    {
      PTBD_PTBD6 = 0;			            // LED = on
      for (temp=3000; temp; temp--);	// wait for a while
      PTBD_PTBD6 = 1;			            // LED = off
      for (temp=30000; temp; temp--);	// wait for a while
    }
    IRQSC_IRQIE = 1;	                // enable IRQ interrupt
  }
}


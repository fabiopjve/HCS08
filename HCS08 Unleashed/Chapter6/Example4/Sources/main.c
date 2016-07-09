// DEMO9S08QE128 Event counter using the IRQ pin
// LEDS are connected to port C pins
// RESET/IRQ key is connected to PTA5
#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

unsigned char counter=0;

// This is the IRQ interrupt servicing routine
void interrupt VectorNumber_Virq IRQ_isr()
{
  IRQSC_IRQACK = 1;	          // acknowledge IRQ interrupt (clear IRQF)
  counter++;		              // increment counter
}

void main(void) 
{
  SOPT1 = bBKGDPE;            // configure SOPT1 register, enable pin BKGD for BDM
  PTCDD = 0xFF;		            // PTC0 through PTB7 as outputs
  PTEDD = BIT_7 | BIT_6;	    // PTE7 and PTE6 as outputs
  PTAPE = BIT_5;	            // PTA5 pull up enabled
  IRQSC = bIRQPE | bIRQIE;	  // IRQ pin enabled, IRQ interrupt enabled
  EnableInterrupts;     	    // enable interrupts (CCR:I = 0)
  while (1)
  {
    PTCD = ~counter;	        // PTCD = negated counter (all bits inverted)
    PTED_PTED7 = PTCD_PTCD7;	// PTE7 state = PTC7 state
    PTED_PTED6 = PTCD_PTCD6;	// PTE6 state = PTE6 state
  }
}



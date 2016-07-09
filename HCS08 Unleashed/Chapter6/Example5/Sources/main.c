// DEMO9S08QG8 KBI-controlled led flasher
// LED1 cathode is connected to PTB6, LED2 cathode is connected to PTB7
// SW1 is connected to PTA2, SW2 is connected to PTA3
 
#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

unsigned int blinking_rate = 15000;

#define LED1	PTBD_PTBD6
#define LED2	PTBD_PTBD7
#define SW1	PTAD_PTAD2
#define SW2	PTAD_PTAD3

/*
  This is the KBI interrupt servicing routine
  SW1 and SW2 are falling edge sensitive (interrupt is triggered when key is pressed 
*/
void interrupt VectorNumber_Vkeyboard KBI_isr()
{
  KBISC_KBACK = 1; 	// acknowledge KBI interrupt (clear KBF)
  if (!SW1) blinking_rate = 15000; // if SW1 is pressed, change blinking rate
  if (!SW2) blinking_rate = 30000; // if SW2 is pressed, change blinking rate
}

void main(void) 
{
  unsigned int temp;
  SOPT1 = bBKGDPE;		// configure SOPT1 register, enable pin BKGD for BDM
  PTADD = 0;			    // all PTA pins as inputs
  PTBDD = 0xFE;			  // pin PTB0 as input, PTB1 through PTB7 as outputs
  PTBD = 0xC0;			  // turn off LED1 and LED2, clear other PTB outputs
  PTAPE = BIT_5 | BIT_3 | BIT_2;  // PTA5, PTA3 and PTA2 pull ups enabled
  KBIPE = BIT_3 | BIT_2; 	        // KBIP3 (PTA3) and KBIP2 (PTA2) inputs enabled
  KBISC = bKBIE;  		// KBI interrupt enabled
  EnableInterrupts;		// enable interrupts (CCR:I = 0)
  while (1)
  {
    LED1 = 0; LED2 = 1;		                	// LED1 = on, LED2 = off
    for (temp=blinking_rate; temp; temp--);	// wait for a while
    LED1 = 1; LED2 = 0;		                  // LED1 = off, LED2 = on
    for (temp=blinking_rate; temp; temp--);	// wait for a while
  }
}



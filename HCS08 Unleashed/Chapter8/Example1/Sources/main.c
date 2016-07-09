// DEMO9S08QG8 stop3 mode example
// LED1 cathode is connected to PTB6
// LED2 cathode is connected to PTB7
// SW1 is connected to PTA2
// SW2 is connected to PTA3

#include <hidef.h> 	      /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  	    // This is our definition file !

#define LED1  	PTBD_PTBD6
#define LED2  	PTBD_PTBD7
#define SW1	PTAD_PTAD2
#define SW2	PTAD_PTAD3

unsigned char mode;	// current mode 

// A simple software loop delay
void delay(unsigned int value)
{
  for (;value; value--);
}

// This is the KBI interrupt servicing routine
void interrupt VectorNumber_Vkeyboard KBI_isr()
{
  KBISC_KBACK = 1; 	// acknowledge KBI interrupt (clear KBF)
  if (!SW2) LED2 = !LED2;	// if SW2 is pressed then toggle LED2 state
  if (!SW1) mode = !mode;	// if SW1 is low (pressed) then toggle mode
}

void main(void) 
{
  // configure SOPT1 register, enable STOP instruction and BKGD pin
  SOPT1 = bSTOPE | bBKGDPE;
  SPMSC2 = 0;			        // stop3 mode selected
  PTBDD = 0xFF;			      // all PTB pins as outputs	
  PTAPE = BIT_2 | BIT_3;	// turn on internal pull ups on PTA2, PTA3 
  KBISC = bKBIE;		      // enable KBI interrupt
  KBIPE = BIT_2 | BIT_3;	// enable KBI inputs 2 and 3 (PTA2 and PTA3)
  mode = 0;
  EnableInterrupts;		    // enable interrupts (CCR:I = 0)
  while (1)
  {
    LED1 = 0;         		// LED1 is on
    delay(30000);     		// delay
    LED1 = 1;         		// LED1 is off
    delay(30000);     		// delay
    // if mode is true (different from zero) then enter stop3 mode
    if (mode)
    {
      STOP;		            // enter stop3 mode
      mode = 0;		// after wakeup, return to mode = 0 (stop mode disabled)
    }
  }
}




// DEMO9S08AW60E KBI-controlled led sequencer
// SW1 is connected to PTC2
// SW2 is connected to PTC6
// SW3 is connected to PTD3
// SW4 is connected to PTD2
// the LEDs are connected to PTF
 
#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

unsigned int blinking_rate = 15000;
unsigned char shift_dir = 0;

#define SW4		PTDD_PTDD2
#define SW3		PTDD_PTDD3

// This is the KBI interrupt servicing routine
void interrupt VectorNumber_Vkeyboard1 KBI_isr()
{
  KBI1SC_KBACK = 1; 	// acknowledge KBI interrupt (clear KBF)
  if (!SW3)		        // if PTD3 is equal to "0"
  {
    // SW3 is pressed, change blinking rate
    if (blinking_rate == 30000) blinking_rate = 15000; else blinking_rate = 30000;
  }
  if (!SW4)	        	// if PTD2 is equal to "0"
  {
    // SW4 is pressed, change shift direction
    shift_dir = !shift_dir;
  }
}

void main(void) 
{
  unsigned int temp;
  unsigned char shift_register = 1;
  SOPT = 0;                 // configure SOPT1 register, STOP instruction and COP disabled
  PTCDD = ~(BIT_2 | BIT_6);	// PTC2 and PTC6 as inputs, remaining pins as outputs
  PTCPE = BIT_2 | BIT_6;	  // PTC2 and PTC6 internal pull ups enabled
  PTDDD = ~(BIT_2 | BIT_3);	// PTD2 and PTD3 as inputs, remaining pins as outputs
  PTDPE = BIT_2 | BIT_3;	  // PTD2 and PTD3 internal pull ups enabled
  PTFDD = 0xFF;	            // all PTF pins as outputs
  KBI1PE = BIT_5 | BIT_6;	  // KBIP5 (PTD2) and KBIP6 (PTD3) inputs enabled
  KBI1SC = bKBIE;	          // KBI interrupt enabled
  EnableInterrupts;	        // enable interrupts (CCR:I = 0)
  while (1)
  {
    PTFD = shift_register;  			          // change led states
    for (temp=blinking_rate; temp; temp--);	// wait for a while
    if (shift_dir)  
    {      
      shift_register <<= 1;   // shifting to right
      // did we reached the last led?
      if (!shift_register) shift_register=1;
    }
 
    else
    {
      shift_register >>= 1;   // shifting to left
      // did we reached the last led?
      if (!shift_register) shift_register=128;
    }
  }
}




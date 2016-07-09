// DEMO9S08QG8 Key-controlled Led flasher
#include <hidef.h> 		/* for EnableInterrupts macro */
#include "derivative.h" 		/* include peripheral declarations */
#include "hcs08.h"  		// This is our definition file!

void main(void) 
{
  unsigned int temp;
  unsigned char blinking=0;
  SOPT1 = bBKGDPE;	// configure SOPT1 register, enable pin BKGD for BDM
  PTADD = 0;		// all PTA pins as inputs
  PTBDD = 0xFE;	// pin PTB0 as input, PTB1 through PTB7 as outputs
  PTBD = 0xC0;	// turn off LED1 and LED2, clear other PTB outputs
  PTAPE = BIT_2;	// PTA2 pull up enabled
  while (1)
  {
    if (blinking)
    {
      PTBD_PTBD6 = 0;	// LED = on
      for (temp=3000; temp; temp--);	// wait for a while
      PTBD_PTBD6 = 1;	// LED = off
      for (temp=30000; temp; temp--);	// wait for a while
    }
    if (!PTAD_PTAD2)	// is SW1 pressed ?
    {
      // wait until SW1 is released:
      while (!PTAD_PTAD2) for (temp=100; temp; temp--);
      blinking = !blinking; 	// change blinking state
    }
  }
}

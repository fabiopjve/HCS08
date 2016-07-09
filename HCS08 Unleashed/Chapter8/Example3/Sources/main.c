// DEMO9S08QG8 stop1 mode example
// LED1 cathode is connected to PTB6, LED2 cathode is connected to PTB7
// SW1 is connected to PTA2, SW2 is connected to PTA3
// RESET is connected to PTA5/IRQ/RESET

#include <hidef.h> 	      /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  	    // This is our definition file!

#define LED1  	PTBD_PTBD6
#define LED2  	PTBD_PTBD7
#define SW1	PTAD_PTAD2
#define SW2	PTAD_PTAD3

// A simple software loop delay
void delay(unsigned int value)
{
  for (;value; value--);
}

// This is the KBI interrupt servicing routine
void interrupt VectorNumber_Vkeyboard KBI_isr()
{
  KBISC_KBACK = 1; 		    // acknowledge KBI interrupt (clear KBF)
  if (!SW2)	LED2 = !LED2;	// if SW2 is pressed then toggle LED2 state
  if (!SW1) STOP;		      // if SW1 is pressed then enter stop1 mode
}

void main(void) 
{
  // configure SOPT1 register, enable STOP instruction and BKGD pin
  SOPT1 = bSTOPE | bBKGDPE;
  SPMSC1 = 0;             	// disable LVD
  SPMSC2 = bPDC;          	// stop1 mode selected
  PTBDD = 0xFF;           	// PTB pins as outputs
  PTBD = 0xFF;            	// Set all PTB outputs 
  PTAPE = BIT_2 | BIT_3;  	// Turn on pull ups on PTA2 and PTA3
  KBISC = bKBIE;          	// KBI interrupt enabled
  KBIPE = BIT_2 | BIT_3;  	// PTA2 and PTA3 as KBI inputs
  if (SPMSC2_PDF)         	// if PDF is set
  {
    SPMSC2_PPDACK = 1;    	// acknowledge PDF flag
    LED2 = 0;             	// turn LED2 on
  }
  EnableInterrupts;       	// enable interrupts (CCR:I = 0)
  while (1)
  {
    LED1 = 0;         	// LED1 is on
    delay(30000);     	// delay
    LED1 = 1;         	// LED1 is off
    delay(30000);     	// delay
  }
}


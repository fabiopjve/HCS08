// DEMO9S08QG8 RTI in stop2 mode example
// LED1 cathode is connected to PTB6
// LED2 cathode is connected to PTB7

#include <hidef.h> 	      /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  	    // This is our definition file!

#define LED1  	  PTBD_PTBD6
#define LED2    	PTBD_PTBD7
#define TIMEOUT 	8

#pragma DATA_SEG MY_NO_INIT_SEG
unsigned char led_state, counter;

#pragma DATA_SEG DEFAULT
volatile char aux, temp;

void main(void) 
{
  // configure SOPT1 register, enable STOP instruction and BKGD pin
  SOPT1 = bSTOPE | bBKGDPE;
  SPMSC1 = 0;		          // disable LVD
  SPMSC2 = bPDC | bPPDC;	// stop2 mode selected
  PTBDD = 0xFF;		        // PTB pins as outputs
  PTBD = 0xFF;		        // Set all PTB outputs 
  

  
  // enable RTI (internal clock source, 512ms timeout, no interrupt)
  SRTISC = RTI_512ms;	
  if (SPMSC2_PPDF)        	// if PPDF is set
  {
    SPMSC2_PPDACK = 1;    	// acknowledge PPDF flag
    if (!--counter)	        // decrement counter and if it is zero...
    {
      if (led_state)	      // if led_state is true
      {
        LED1 = 0;	          // turn LED1 on
        LED2 = 1;	          // turn LED2 off
      } else		            // if led_state is false
      {
        LED1 = 1;	          // turn LED1 off
        LED2 = 0;    	      // turn LED2 on
      }
      aux++;	            	// this is just a dummy variable
      led_state = !led_state;	// invert led_state
      counter = TIMEOUT;	  // restart counter
    }
  } else counter = TIMEOUT;	// if PPDF=0, restart counter
  STOP;		                  // enter stop1 mode
  // the program does not continue past the STOP instruction above!
}


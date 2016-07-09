// Two LED flasher and key reading through a simple pin (tested on an MC9S08QG8)
#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

// delay function
void delay(unsigned int temp)
{
  for (;temp;temp--);
}

// Turn LED1 on
#pragma inline
void led1on(void)
{
  PTAD = 0; 	      // turn LED1 on
  PTADD_PTADD0 = 1;	// PTA0 as output
}

// Turn LED2 on
#pragma inline
void led2on(void)
{
  PTAD = 1; 	      // turn LED2 on
  PTADD_PTADD0 = 1;	// PTA0 as output
}

// Turn LED1 and LED2 off
#pragma inline
void ledsoff(void)
{
  PTADD_PTADD0 = 0; 	// PTA0 as input (turn leds off)
}

// Return key state (0 = pressed, 1 = not pressed)
#pragma inline
char read_key(void)
{
  PTADD_PTADD0 = 0; 	// PTA0 as input (turn leds off)
  if (PTAD & 0x01) return(1); else return(0);  
}

void main(void) 
{
  unsigned int aux;
  SOPT1 = bBKGDPE;	  // Enable debug pin
  PTAPE_PTAPE0 = 1;	  // PTA0 pull up enabled
  PTADS_PTADS0 = 1;	  // Extra driving strength for PTA0
  aux = 40000;	      // initial delay for leds
  while (1)
  {
    led1on();	        // turn LED1 on
    delay(aux);	      // wait for a while
    led2on();	        // turn LED2 on
    delay(aux);	      // wait for a while
    ledsoff();	      // turn both leds off
    delay(aux);	      // wait for a while
    // read key and set blinking rate accordingly (leds blink faster when
    // the key is pressed
    if (read_key()) aux = 40000; else aux = 10000;
  }
}


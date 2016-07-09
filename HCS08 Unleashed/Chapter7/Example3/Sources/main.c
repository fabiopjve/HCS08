// DEMO9S08QE128 ICS example (FEI mode, BUSCLK = 24 MHz)

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1 BIT_0      	// PTC0
#define LED2 BIT_1      	// PTC1

// A simple software loop delay
void delay(unsigned int value)
{
  for (;value; value--);
}

void main(void) 
{
  SOPT1 = bBKGDPE;			      // configure SOPT1 register, enable pin BKGD for BDM
  ICSSC = DCO_HIGH | NVFTRIM;	// configure FTRIM value, select DCO high range
  ICSTRM = NVICSTRM;			    // configure TRIM value
  ICSC1 = ICS_FLL | bIREFS;  	// select FEI mode (ICSOUT = DCOOUT = 1536 * IRCLK)
  ICSC2 = BDIV_1;             // ICSOUT = DCOOUT / 1
  PTCDD = 0xFF;               // PTC pins as outputs
  PTCD = 0xFE;                // set all PTC pins, except PTC0 (LED1 is on, LED2 is off)
  while (1)
  {
    PTCTOG = LED1;  // toggle LED1 (bit 0 of PTC)
    PTCTOG = LED2;  // toggle LED2 (bit 1 of PTC)
    delay(60000);   // a big delay
    PTCTOG = LED1;  // toggle LED1 (bit 0 of PTC)
    PTCTOG = LED2;  // toggle LED2 (bit 1 of PTC)
    delay(60000);   // a big delay
  }
}


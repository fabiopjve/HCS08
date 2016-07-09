// DEMO9S08QE128 low power run example (FBELP mode, BUSCLK = 16384 Hz)
// LED1 is connected to PTC0, LED2 is connected to PTC1, LED3 is connected to PTC2
// SW1 is connected to PTA2, SW2 is connected to PTA3

#include <hidef.h> 	      /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  	    // This is our definition file !

#define LED1 BIT_0      	// PTC0
#define LED2 BIT_1      	// PTC1
#define LED3 BIT_2      	// PTC2
#define SW1  PTAD_PTAD2 
#define SW2  PTAD_PTAD3

// A simple software loop delay
void delay(unsigned int value)
{
  for (;value; value--);
}

void main(void) 
{
  SOPT1 = bBKGDPE;		  // configure SOPT1 register, enable pin BKGD for BDM
  ICSC2 = bEREFS;   		// disable FLL and select the external oscillator
  ICSSC = DCO_MID | bDMX32; 	// select DCO mid-range and ajusts it to 32768 Hz
  ICSC1 = ICS_FLL;  		// ICS in FEE mode (FLL engaged external because IREFS = 0)
  while (ICSSC_CLKST != ICSC1_CLKS);	// wait for the clock change to take effect
  SPMSC1 = 0;       		// disable LVD module
  PTCDD = 0xFF;     		// PTC pins as outputs
  PTCD = 0xFE;      		// set all PTC pins, except PTC0 (LED1 is on, LED2 is off)
  PTAPE = BIT_2 | BIT_3;   	// PTA2 and PTA3 internal pull ups enabled
  while (1)
  {
    PTCTOG = LED1;  		// toggle LED1 (bit 0 of PTC)
    PTCTOG = LED2;  		// toggle LED2 (bit 1 of PTC)
    delay(60);      		// a little delay
    PTCTOG = LED1;  		// toggle LED1 (bit 0 of PTC)
    PTCTOG = LED2;  		// toggle LED2 (bit 1 of PTC)
    delay(60);      		// a little delay
    // now we test LPRS and turn LED3 on if it is set (low power run mode is enabled) 
    if (SPMSC2_LPRS) PTCCLR = LED3; else PTCSET = LED3;
    if (!SW1)
    {
      ICSC2 = bICS_LP | bEREFS; // FLL low power mode, crystal oscillator
      ICSC1 = ICS_FBE;        	// ICS in FBE mode (BUSCLK = 16384 Hz)
      SPMSC2_LPR = 1;   	      // enter low power run mode
    }

        if (!SW2)
    {
      ICSC2 = bEREFS;   	// External crystal oscillator 
      ICSC1 = ICS_FLL;  	// ICS in FEE mode (BUSCLK = 20MHz)
      SPMSC2_LPR = 0;   	// exit low power run mode 
    }
  }
}


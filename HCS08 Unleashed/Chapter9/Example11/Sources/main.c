// DEMO9S08AW60 - TPM PWM example 1
// LEDs connected to PTF0, PTF1 and PTF2

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define NVICGTRM (*(const char * __far)0x0000FFBE)

void main(void) 
{
  SOPT = 0;		            // configure SOPT1 register, disable COP
  // ICG configuration (ICGOUT = 40MHz, BUSCLK = 20MHz):
  ICGTRM = NVICGTRM;	    // trim IRG oscillator
  ICGC2 = MFDx18;	        // MFD = 111b (N = 18), RFD = 000b (R = 1)
  ICGC1 = ICG_FEI;	      // ICG in FEI mode (CLKS = 01b)
  TPM1SC = TPM_BUSCLK;	  // TPM clock = BUSCLK = 20MHz
  TPM1MOD = 2047;	        // PWM frequency is equal to 9.77 kHz
  // Configure channels 2,3 and 4 to PWM mode (high-true pulses)
  TPM1C2SC = TPM_PWM_HIGH;
  TPM1C3SC = TPM_PWM_HIGH;
  TPM1C4SC = TPM_PWM_HIGH;
  TPM1C2V = 19;		        // channel 2 set to 1%
  TPM1C3V = 511;	        // channel 3 set to 25%  
  TPM1C4V = 1535;	        // channel 4 set to 75%
  while (1);
}

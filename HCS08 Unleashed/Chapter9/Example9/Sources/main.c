// DEMO9S08AW60 - TPM compare example (toggle output)
// LED connected to PTF0

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define NVICGTRM (*(const char * __far)0x0000FFBE)

void interrupt VectorNumber_Vtpm1ch2 tpm1ch2_isr(void)
{
  TPM1C2SC_CH2F = 0; 	// clear interrupt flag
  TPM1C2V += 1000;		// adds 1000 to TPM1C2V (sets next compare)
}

void main(void) 
{
  SOPT = bBKGDPE;		    // configure SOPT1 register, enable pin BKGD for BDM
  // ICG configuration (ICGOUT = 40MHz, BUSCLK = 20MHz):
  ICGTRM = NVICGTRM;		// trim IRG oscillator
  ICGC2 = MFDx18;   		// MFD = 111b (N = 18), RFD = 000b (R = 1)
  ICGC1 = ICG_FEI;		  // ICG in FEI mode (CLKS = 01b)
  TPM1SC = TPM_BUSCLK | TPM_DIV128;  // TPM clock = BUSCLK / 128 = 156250Hz
  // Configure channel 2 to compare mode (toggle output)
  TPM1C2SC = bCHIE | TPM_COMPARE_TOGGLE;
  EnableInterrupts;		  // enable interrupts
  while (1);
}

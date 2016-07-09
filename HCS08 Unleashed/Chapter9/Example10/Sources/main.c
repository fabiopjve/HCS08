// DEMO9S08LC60 - TPM capture/compare example
// SW302 is connected to PTC3

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED PTBD_PTBD7
unsigned int key_time;

// TPM1 channel 1 isr (process the channel captures)
void interrupt VectorNumber_Vtpm1ch1 tpm1ch1_isr(void)
{
  static unsigned int last_capture;
  static char mode;
  TPM1C1SC_CH1F = 0; 	        // clear interrupt flag
  if (!mode)
  { // if mode=0 then we have a falling edge capture (the key was just pressed)
    last_capture = TPM1C1V; 	// store the current capture
    // change channel mode to capture the key release (rising edge)
    TPM1C1SC = bCHIE | TPM_CAPTURE_RISING_EDGE;
    mode++;   			          // mode = 1;
  } else
  { // if mode<>0 then we have a rising edge capture (the key was just released)
    key_time = TPM1C1V - last_capture;  // calculate key press time
    // change channel mode to capture the next key press (falling edge)
    TPM1C1SC = bCHIE | TPM_CAPTURE_FALLING_EDGE;
    mode = 0;
  }
}

void interrupt VectorNumber_Vtpm1ch0 tpm1ch0_isr(void)
{
  TPM1C0SC_CH0F = 0;    	// clear interrupt flag
  TPM1C0V += key_time;  	// update the new cycle
  LED = !LED;           	// toggle led state
}

void init(void)
{
  SOPT1 = bBKGDPE;		    // configure SOPT1 register, enable pin BKGD for BDM
  // ICG configuration (ICGOUT = 10MHz, BUSCLK = 5MHz):
  ICGTRM = NVICGTRM;	    // trim IRG oscillator
  ICGC2 = MFDx18 | RFD_DIV4;   	// MFD = 111b (N = 18), RFD = 010b (R = 4)
  ICGC1 = ICG_FEI;	// ICG in FEI mode (CLKS = 01b)
  PTCDD_PTCDD3 = 0;   		// PTC3 as an input
  PTBDD_PTBDD7 = 1;   		// PTB7 as an output
  TPM1SC = TPM_BUSCLK | TPM_DIV128;	// TPM clock = BUSCLK / 128 = 39,062.5Hz
  // Configure channel 0 to compare mode (interrupt only)
  TPM1C0SC = bCHIE | TPM_COMPARE_INT;
  // Configure channel 1 to capture the next key press (falling edge)
  TPM1C1SC = bCHIE | TPM_CAPTURE_FALLING_EDGE;
  EnableInterrupts;   		// enable interrupts
}

void main(void) 
{
  init();
  while (1);
}

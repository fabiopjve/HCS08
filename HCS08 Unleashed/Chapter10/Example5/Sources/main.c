// DEMO9S08QG8 - ADC example 2 (interrupts)
// LED1 connected to PTB6, LED2 connected to PTB7
// RV1 connected to PTA0, RZ connected to PTA1

#include <hidef.h>      	/* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1  PTBD_PTBD6
#define LED2  PTBD_PTBD7

unsigned int result;

// ADC ISR (store the last conversion result)
void interrupt VectorNumber_Vadc adc_isr(void)
{
  result = ADCR;  	      // read the result (clear interrupt flag)
}

void main(void) 
{
  SOPT1 = bBKGDPE;		    // configure SOPT1 register, enable pin BKGD for BDM
  ICSSC = NV_FTRIM;		    // configure FTRIM value
  ICSTRM = NV_ICSTRM;	    // configure TRIM value
  ICSC2 = 0;			        // ICSOUT = DCOOUT / 1 (16MHz)
  // enable long sampling, 10-bit mode, ADICLK = 11b, ADCK = BUSCLK/2
  ADCCFG = bADLSMP | ADC_10BITS | ADC_BUSCLK_DIV2 | ADIV_8;
  APCTL1 = BIT_0;		      // ADP0 in analog mode
  ADCSC1 = bAIEN | bADCO | ADC_CH0; // ADC in continuous mode, channel 0, interrupts enabled
  PTBDD = BIT_6 | BIT_7;	// PTB6 and PTB7 as outputs
  EnableInterrupts;		    // Enable interrupts (CCR:I=0)  
  while (1)
  {
    if (result>512) LED1 = 0; else LED1 = 1;
    if (result>768) LED2 = 0; else LED2 = 1;
  }
}

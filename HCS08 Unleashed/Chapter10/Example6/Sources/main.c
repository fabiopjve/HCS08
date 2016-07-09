// DEMO9S08QG8 - ADC example 3 (sequential readings)
// LED1 connected to PTB6, LED2 connected to PTB7
// RV1 connected to PTA0, RZ connected to PTA1
#include <hidef.h>      	/* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1  PTBD_PTBD6
#define LED2  PTBD_PTBD7

unsigned int trimpot, sensor;

// ADC ISR (store the conversion result for RV1 or RZ)
void interrupt VectorNumber_Vadc adc_isr(void)
{
  if (ADCSC1 & 0x1F)
  {
    sensor = ADCR;            // read the channel 1 result
    ADCSC1 = bAIEN | ADC_CH0; // next channel = 0
  } 
  else
  {
    trimpot = ADCR;           // read the channel 0 result
    ADCSC1 = bAIEN | ADC_CH1; // next channel = 1
  }  
}

void main(void) 
{
  SOPT1 = bBKGDPE;		      // configure SOPT1 register, enable pin BKGD for BDM
  ICSSC = NV_FTRIM;		      // configure FTRIM value
  ICSTRM = NV_ICSTRM;	      // configure TRIM value
  ICSC2 = 0;			          // ICSOUT = DCOOUT / 1
  // enable long sampling, 10-bit mode, ADICLK = 11b, ADCK = BUSCLK/2
  ADCCFG = bADLSMP | ADC_10BITS | ADC_BUSCLK_DIV2 | ADIV_8;
  APCTL1 = BIT_0 | BIT_1;	  // ADP0 and ADP1 in analog mode
  // ADC channel 0, interrupts enabled  
  ADCSC1 = bAIEN | ADC_CH0; 
  PTBDD = BIT_6 | BIT_7;	  // PTB6 and PTB7 as outputs
  EnableInterrupts;		      // Enable interrupts (CCR:I=0)  
  while (1)
  {
    if (trimpot>512) LED1 = 0; else LED1 = 1;
    if (sensor>trimpot) LED2 = 0; else LED2 = 1;
  }
}

// DEMO9S08QG8 - ADC example 4 (ADC comparator and stop mode)
// LED1 connected to PTB6, LED2 connected to PTB7
// RV1 connected to PTA0, RZ connected to PTA1

#include <hidef.h>      	/* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1  PTBD_PTBD6

unsigned int sensor;

// ADC ISR
void interrupt VectorNumber_Vadc adc_isr(void)
{
  sensor = ADCR;
  if (ADCSC2_ACFGT)   // if adc comparator mode is higher than
  {
    LED1 = 0;         // turn LED1 on
    ADCSC2_ACFGT = 0; // set ADC comparator to less than mode
  }
  else                // if adc comparator mode is less than
  {
    LED1 = 1;         // turn LED1 off
    ADCSC2_ACFGT = 1; // set ADC comparator to greater than or equal to mode
  }
  ADCSC1_ADCH = 1;    // restart ADC conversion  
}

void main(void) 
{
  // configure SOPT1 register, enable pin BKGD for BDM and STOP instruction
  SOPT1 = bSTOPE | bBKGDPE;
  ICSSC = NV_FTRIM;         // configure FTRIM value
  ICSTRM = NV_ICSTRM;       // configure TRIM value
  ICSC2 = 0;                // ICSOUT = DCOOUT / 1
  SPMSC2 = 0;               // stop3 mode selected
  // enable long sampling, 10-bit mode, ADICLK = 11b, ADCK = ADACK
  ADCCFG = bADLSMP | ADC_10BITS | ADC_INTCLK | ADIV_8;
  ADCCV = 900;              // compare value for the ADC
  ADCSC2 = bACFE | bACFGT;  // ADC comparator enabled (greater than or equal to)   
  APCTL1 = BIT_1;           // ADP1 in analog mode
  // ADC in continuous mode, channel 1, interrupts enabled  
  ADCSC1 = bAIEN | bADCO | ADC_CH1;  
  PTBDD = BIT_6;            // PTB6 as output
  LED1 = 1;                 // turn LED1 off
  EnableInterrupts;         // Enable interrupts (CCR:I=0)  
  while (1)
  {
    STOP;                   // enter stop3 mode
  }
}

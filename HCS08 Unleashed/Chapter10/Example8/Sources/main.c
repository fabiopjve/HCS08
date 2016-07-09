// DEMO9S08QG8 - ADC example 5 (using the ADC as a simple timer)

#include <hidef.h>      	/* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1  PTBD_PTBD6

unsigned int timeout;

// ADC interrupt
void interrupt VectorNumber_Vadc adc_isr(void)
{
  char temp;
  temp = ADCRL;   // dummy read of ADCR (only the low byte) to clear the interrupt flag
  if (timeout) timeout--;
}

// ADC delay function (each "delay" unit is equal to 80us)
void adc_delay(unsigned int delay)
{
  timeout = delay;
  while (timeout);  
}

void main(void) 
{
  // configure SOPT1 register, enable pin BKGD
  SOPT1 = bBKGDPE;
  ICSSC = NV_FTRIM;         // configure FTRIM value
  ICSTRM = NV_ICSTRM;       // configure TRIM value
  ICSC2 = 0;                // ICSOUT = DCOOUT / 1
  // enable long sampling, 10-bit mode, ADICLK = 11b, ADCK = BUSCLK
  // ADC clock = 500 kHz
  ADCCFG = bADLSMP | ADC_10BITS | ADC_BUSCLK_DIV2 | ADIV_8;
  // ADC in continuous mode, channel 30 (VREFL), interrupts enabled
  // one conversion each 40 ADC clocks = 80us 
  ADCSC1 = bAIEN | bADCO | ADCH_REFH;  
  PTBDD = BIT_6;            // PTB6 as output
  EnableInterrupts;         // Enable interrupts (CCR:I=0)  
  while (1)
  {
    LED1 = 0;         // LED1 on
    adc_delay(6250);  // wait for 500ms
    LED1 = 1;         // LED1 off
    adc_delay(6250);  // wait for 500ms
  }
}

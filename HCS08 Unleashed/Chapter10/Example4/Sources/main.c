// DEMO9S08QG8 - ADC example 1
// LED1 connected to PTB6
// LED2 connected to PTB7
// RV1 connected to PTA0
// RZ connected to PTA1

#include <hidef.h>      	/* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1  PTBD_PTBD6
#define LED2  PTBD_PTBD7

unsigned int result;

void main(void) 
{
  SOPT1 = bBKGDPE;	 // configure SOPT1 register, enable pin BKGD for BDM
  ICSSC = NV_FTRIM;               // configure FTRIM value
  ICSTRM = NV_ICSTRM;             // configure TRIM value
  ICSC2 = 0;                      // ICSOUT = DCOOUT / 1
  ADCCFG = bADLSMP | ADC_10BITS;  // enable long sampling, 10-bit mode
  APCTL1 = BIT_0;                 // ADP0 in analog mode
  ADCSC1 = bADCO | ADC_CH0;       // ADC in continuous mode, channel 0
  PTBDD = BIT_6 | BIT_7;          // PTB6 and PTB7 as outputs  
  while (1)
  {
    if (ADCSC1_COCO)              // if a new conversion is complete
    {
      result = ADCR;              // read the result (clear COCO)
      if (result>512) LED1 = 0; else LED1 = 1;
      if (result>768) LED2 = 0; else LED2 = 1;
    }
  }
}


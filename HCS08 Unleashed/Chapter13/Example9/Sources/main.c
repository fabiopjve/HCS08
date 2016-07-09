/*
  Tilt indicator with the MC9S08QE128 and MMA7260QT (DEMO9S08QE128 board)

  By Fábio Pereira (01/20/08)
  Joinville - SC - Brasil
  www.sctec.com.br
 	
	PTA1 - XOUT
	PTA6 - YOUT
	PTA7 - ZOUT
	PTC4 - MMA7260 sleep mode selection (0 = sleep, 1 = operating)
	PTD0 - MMA7260 g-select1 input
	PTD1 - MMA7260 g-select2 input
	PTC0 to PTC5 - LEDs
	PTE6, PTE7 - LEDs

 	ADC:
	- accelerometer measurements (PTA1, PTA6 and PTA7)
*/

#include <hidef.h>      	/* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  	    // This is our definition file!

#define ACCEL_ON    PTCD_PTCD4
#define CAL_SW      PTDD_PTDD3

unsigned int x_axis, y_axis, z_axis;
unsigned long int x_sum, y_sum, z_sum;
unsigned int x_cal;
char adc_seq_complete;

// ADC ISR
void interrupt VectorNumber_Vadc adc_isr(void)
{
  static char adc_seq;
  switch (adc_seq)
  {
    case 0:
      x_sum = (x_sum + ADCR) - x_axis;
      ADCSC1 = bAIEN | ADC_CH8; // next channel = 8 (y axis)
      adc_seq++;
      break;
    case 1:
      y_sum = (y_sum + ADCR) - y_axis;
      ADCSC1 = bAIEN | ADC_CH9; // next channel = 9 (z axis)
      adc_seq++;
      break;
    case 2:
      z_sum = (z_sum + ADCR) - z_axis;
      ADCSC1 = bAIEN | ADC_CH1; // next channel = 1 (x axis)
      adc_seq = 0;
      adc_seq_complete = 1;   // signal the end of an ADC sequence
      break;
  }  
}

void main(void) 
{
  SOPT1 = bBKGDPE;		// configure SOPT1 register, enable pin BKGD for BDM
  ICSSC = NVFTRIM;		// configure FTRIM value
  ICSTRM = NVICSTRM;		// configure TRIM value
  ICSC2 = 0;			// ICSOUT = DCOOUT / 1
  // enable long sampling, 12-bit mode, ADICLK = 11b, ADCK = BUSCLK/16
  ADCCFG = bADLSMP | ADC_12BITS | ADC_BUSCLK_DIV2 | ADIV_8;
  APCTL1 = bADPC1;         		// ADP1 in analog mode
  APCTL2 = bADPC8 | bADPC9; 	// ADP8 and ADP9 in analog mode
  // ADC channel 1, interrupts enabled  
  ADCSC1 = bAIEN | ADC_CH1;
  PTDD = 0;			// select 1.5g sensitivity
  PTDDD = BIT_0 | BIT_1; 
  PTBDD = BIT_6 | BIT_7;		// PTB6 and PTB7 as outputs
  PTCDD = 0xFF;           		// PTC as outputs
  PTEDD = BIT_7 | BIT_6;
  x_sum = y_sum = z_sum = 0;
  x_axis = y_axis = z_axis = 0;
  ACCEL_ON = 1;
  x_cal = 0;
  EnableInterrupts;		// Enable interruts (CCR:I=0)
  while (1)
  {
    if (adc_seq_complete)
    {
      x_axis = x_sum/32;		// calculate the x axis
      y_axis = y_sum/32;		// calculate the y axis
      z_axis = z_sum/32;		// calculate the z axis
      adc_seq_complete = 0;
    }
    PTCD = 0xFF;		// turn off all LEDs connected to PTC
    PTED = 0xC0;		// turn off all LEDs connected to PTE
    // if the calibration key is pressed, store the current value of the x axis
    if (!CAL_SW) x_cal = x_axis;
    if (x_axis>(x_cal+500))  PTCD_PTCD0 = 0; else
      if (x_axis>(x_cal+350)) PTCD_PTCD1 = 0; else
        if (x_axis>(x_cal+200)) PTCD_PTCD2 = 0; else
          if (x_axis<(x_cal-500)) PTED_PTED7 = 0; else
            if (x_axis<(x_cal-350)) PTED_PTED6 = 0; else
              if (x_axis<(x_cal-200)) PTCD_PTCD5 = 0; else PTCD_PTCD3 = 0;
    
  }
}

/*
  PPM relay controller using the MC9S08QD4
  By Fábio Pereira (12/17/07)
  Joinville - SC - Brasil
  www.sctec.com.br
 	
	PTA0 - PPM input
 	PTA2 - Relay output

 	TPM:
 	- pulse width measurements (detected with KBI interrupt)
*/

#define RELAY   PTAD_PTAD2

#include <hidef.h>
#include "derivative.h"
#include "hcs08.h"

volatile unsigned int pulse_width, average_width, sum_width;
volatile unsigned char new_sample;

#define MY_NVFTRIM  (*(const char * __far)0x0000FFAE)
#define MY_NVICSTRM (*(const char * __far)0x0000FFAF)

// KBI ISR
void interrupt VectorNumber_Vkeyboard1 kbi_isr (void)
{
  static unsigned int last_tpm;
  unsigned int temp;
  KBISC_KBACK = 1;        // clear interrupt flag
  if (KBIES)              // rising edge ?
  {
    last_tpm = TPMCNT;    // store the TPM counting
    KBIES = 0;            // next edge = falling
  } 
  else			      // falling edge ?
  {
    temp = TPMCNT;	      // read current TPM count        
    if (last_tpm>temp)    // if the last TPM counting is higher the the current one
    { 			      // calculate the pulse width
      pulse_width = (4096 - last_tpm) + temp;
    } 
    else                  // if the last TPM counting is lower than the current one
    { 			      // calculate the pulse width
      pulse_width = temp - last_tpm;
    }
    KBIES = BIT_0;        // next edge = rising
    new_sample = 1;       // set the flag
  }
}

// MCU initialization 
void MCU_init(void)
{
  SOPT1 = bBKGDPE;        // enable debug pin
  PTAD = 0;
       // PTA0 as input, PTA1 through PTA5 as outputs
  PTADD = BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5;
  PTADS = BIT_0;          // enable PTA0 drive strength
  SOPT2 = 0;
  // Trim the internal oscillator and config ICS module
  ICSSC = MY_NVFTRIM;     // configure FTRIM value
  ICSTRM = MY_NVICSTRM;   // configure TRIM value
  ICSC2 = 0;              // BDIV = 00b
  // CPU = 16MHz, BUSCLK = ICSCLK/2 = 8MHz
  TPMSC = TPM_BUSCLK | TPM_DIV8;  // TPM clocked by BUSCLK/8 (1MHz)
  TPMMOD = 4095;          // TPM modulo = 4096
  //KBI config
  KBISC = bKBIE;          // KBI interrupt enabled
  KBIPE = BIT_0;          // PTA0 as KBI input
  KBIES = BIT_0;          // rising edge sensitivity
  KBISC_KBACK = 1;        // clear interrupt flag
  // Initialize global variables
  sum_width = 0;
  new_sample = 0;
  average_width = 0;
  EnableInterrupts;       // enable interrupts (CCR:I=0)
}

void main(void) 
{
  MCU_init();
  while (1)
  {
    if (new_sample)       // if a new sample is ready
    {
      // add the new sample to the sum of samples and subtract the average
      sum_width = sum_width + pulse_width - average_width;
      // calculate the new average (the sum divided by 8)
      average_width = sum_width>>3;
      new_sample = 0;     // clear the new_sample flag
    }
    // turn on the relay if the pulse width is greater than 1.8ms
    if (average_width>1800) RELAY = 1;
    // turn off the relay if the pulse width is smaller than 1.3ms
    if (average_width<1300) RELAY = 0;
  }
}

// DEMO9S08AW60 - TPM PWM example (led dimmer)
// SW1 connected to PTC2
// SW2 connected to PTC6
// SW3 connected to PTD3
// SW4 connected to PTD2
// LED is connected to PTF0 (TPM1CH2 pin)

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  	  	// This is our definition file!

#define NVICGTRM (*(const char * __far)0x0000FFBE)
#define SW1 PTCD_PTCD2
#define SW2 PTCD_PTCD6
#define SW3 PTDD_PTDD3
#define SW4 PTDD_PTDD2

unsigned int duty_cycle;

// Key sampler function (called automatically by hardware (RTI))
void interrupt VectorNumber_Vrti key_sampler(void)
{
  static char auto_inc=1, key_timer;
  char flag=0;
  SRTISC_RTIACK = 1;		// clear RTIF
  if (!SW1)			        // if SW1 is pressed
  {
    // decrement the duty cycle by the "auto_inc" amount
    if (duty_cycle>auto_inc) duty_cycle -= auto_inc; else duty_cycle = 0;
    key_timer++;
    flag = 1;			      // set the keypressed flag
  }
  if (!SW2)			        // if SW2 is pressed
  {
    // increment the duty cycle by the "auto_inc" amount
    if (duty_cycle<=TPM1MOD)
    {
      duty_cycle += auto_inc;
      if (duty_cycle>TPM1MOD) duty_cycle = TPM1MOD+1;
    }
    key_timer++;
    flag = 1;			      // set the keypressed flag
  }
  if (key_timer>4)	  	// if the key is pressed for more than 4 samples
  {
    auto_inc++;			    // increment the auto_inc value
    key_timer=5;		    // limit the key_timer variable to 5
  } 
  if (!flag)			      // if SW1 nor SW2 are pressed
  {
    auto_inc = 1;	    	// set auto_inc to 1
    key_timer = 0;	    // clear key_timer
  }
  // if SW3 is pressed, set duty cycle to 0% (led = off)
  if (!SW3) duty_cycle = 0;
  // if SW4 is pressed, set duty cycle to 100% (led = full bright)
  if (!SW4) duty_cycle = TPM1MOD+1;
}

void interrupt VectorNumber_Vtpm1ovf tpm1_overflow_isr(void)
{
  TPM1SC_TOF = 0;       	// clear interrupt flag
  TPM1C2V = duty_cycle; 	// update PWM cycle
}

void main(void) 
{
  SOPT = 0;		          	// configure SOPT1 register, disable COP
  // ICG configuration (ICGOUT = 40MHz, BUSCLK = 20MHz):
  ICGTRM = NVICGTRM;	    // trim IRG oscillator
  ICGC2 = MFDx18;       	// MFD = 111b (N = 18), RFD = 000b (R = 1)
  ICGC1 = ICG_FEI;	      // ICG in FEI mode (CLKS = 01b)
  TPM1SC = bTOIE | TPM_BUSCLK;	// TPM clock = BUSCLK = 20MHz
  TPM1MOD = 255;	      	// PWM frequency is equal to 78.125 kHz
  // Configure channel 2 of TPM1 to PWM mode (active high)
  TPM1C2SC = TPM_PWM_HIGH;	// channel 2 in high-true PWM mode
  TPM1C2V = 0;			      // channel 2 set to 0%
  // Configure the RTI for periodic sampling the keys
  SRTISC = bRTIE | RTI_128ms;
  duty_cycle = 0;
  PTCPE = BIT_2 | BIT_6;
  PTDPE = BIT_2 | BIT_3;
  EnableInterrupts;  
  while (1);
}

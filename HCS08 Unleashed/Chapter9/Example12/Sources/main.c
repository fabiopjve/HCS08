// DEMO9S08AW60 - TPM PWM example 2
// LEDs connected to PTF0 and PTF1

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define NVICGTRM (*(const char * __far)0x0000FFBE)

unsigned int new_cycle[2], timer[2];
unsigned char direction[2];

void interrupt VectorNumber_Vtpm1ch0 tpm1_channel1_isr(void)
{
  TPM1C0SC_CH0F = 0;  		// clear interrupt flag;
  TPM1C0V += 2000;    		// next compare in 100us
  TPM1C0V &= 2047;    		// adjust TPM1C0V to fit the modulo
  if (timer[0]) timer[0]--; else
  { 			                // timer0 timeout
    timer[0] = 15;    		// restart timer0
    if (direction[0]) 		// if direction is 1 (counting down)
    {
      if (new_cycle[0]>12) new_cycle[0] -= 12;  // decrement new_cycle toward 0 
        else direction[0] = 0;  	// if new_cycle[0] is zero, change direction
    } else	// if direction is 0 (counting up) increment new_cycle toward 2048
    {
      if (new_cycle[0]<2048) new_cycle[0] += 12;  
        else direction[0] = 1;  	// if new_cycle[0]>=2048, change direction
    }
  }
  if (timer[1]) timer[1]--; else
  {
    // timer1 timeout
    timer[1] = 37;    		// restart timer1
    if (direction[1]) 		// if direction is 1 (counting down)
    {
      if (new_cycle[1]>12) new_cycle[1] -= 12;  // decrement new_cycle toward 0 
        else direction[1] = 0;  	// if new_cycle[1] is zero, change direction
    } else	// if direction is 0 (counting up) increment new_cycle toward 2048
    {
      if (new_cycle[1]<2048) new_cycle[1] += 12;  
        else direction[1] = 1;  	// if new_cycle[1]>=2048, change direction
    }
  }
}

void interrupt VectorNumber_Vtpm1ovf tpm1_overflow_isr(void)
{
  TPM1SC_TOF = 0; 		// clear interrupt flag
  // update PWM cycles
  TPM1C2V = new_cycle[0];
  TPM1C3V = new_cycle[1];
}

void MCU_init(void)
{
  SOPT = 0;		        // configure SOPT1 register, disable COP
  // ICG configuration (ICGOUT = 40MHz, BUSCLK = 20MHz):
  ICGTRM = NVICGTRM;	// trim IRG oscillator
  ICGC2 = MFDx18;	    // MFD = 111b (N = 18), RFD = 000b (R = 1)
  ICGC1 = ICG_FEI;	  // ICG in FEI mode (CLKS = 01b)
  TPM1SC = bTOIE | TPM_BUSCLK;  // TPM clock = BUSCLK = 20MHz
  TPM1MOD = 2047;	    // PWM frequency is equal to 9.77kHz
  // Configure channel 0 to compare mode (interrupt only)
  TPM1C0SC = bCHIE | TPM_COMPARE_INT;
  // Configure channels 2 and 3 to PWM mode (high-true pulses)
  TPM1C2SC = TPM_PWM_HIGH;
  TPM1C3SC = TPM_PWM_HIGH;
  TPM1C0V = 1999;	    // next compare in 100us ((1999+1)*(1/20MHz))
  TPM1C2V = 0;		    // channel 2 set to 0%
  TPM1C3V = 0;		    // channel 3 set to 0%
  new_cycle[0] = 0;
  new_cycle[1] = 0;
  timer[0] = 15;	    // timer0 timeout is set to 15
  timer[1] = 37;	    // timer1 timeout is set to 37
  direction[0] = 0;
  direction[1] = 0;  
  EnableInterrupts;   // enable interrupts  
}

void main(void) 
{
  MCU_init();
  while (1);
}

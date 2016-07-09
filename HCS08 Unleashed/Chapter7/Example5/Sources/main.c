// DEMO9S08AW60E ICG example (FEE mode, 4 MHz quartz crystal, BUSCLK = 20 MHz)
// SW1 is connected to PTC2, SW2 is connected to PTC6
// SW3 is connected to PTD3, SW4 is connected to PTD2
// LEDs are connected to PTF

#include <hidef.h> 	      /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  	    // This is our definition file !

#define LED7 PTFD_PTFD7
#define NVICGTRM (*(const char * __far)0x0000FFBE)

unsigned char my_mfd, icg_lols;

// ICG interrupt (process LOLS and LOCS interrupts)
void interrupt VectorNumber_Vicg icg_isr(void)
{
  // if it was a loss of clock, we need to restore FEE mode
  if (ICGS1_LOCS)
  {
    // ICG in FEE mode (CLKS = 11), external crystal:
    ICGC1 = ICG_FEE | bREFS | bICG_RANGE;  
    ICGC2 = MFDx10;       	// MFD = 111 (N = 10), RFD = 000 ( R = 1)
    while (!ICGS2_DCOS);  	// wait for the DCO to stabilize
    my_mfd = 3;           	// restore my_mfd factor
  }
  // if it was a loss of lock, flag LOLS condition
  if (ICGS1_LOLS) icg_lols = 1; 
  ICGS1_ICGIF = 1;  	      // clear ICG interrupt  
}

// KBI ISR (process SW3 and SW4 key presses)
void interrupt VectorNumber_Vkeyboard1 kbi1_isr(void)
{
  KBI1SC_KBACK = 1;	        // clear KBI interrupt flag
  if (!PTDD_PTDD3)	        // if SW3 is pressed, increase MFD (speed up BUSCLK)
  { 
    my_mfd++;
    if (my_mfd>3) my_mfd = 3;	// limit my_mfd to 3 (N = 10)
  }


  if (!PTDD_PTDD2)	        // if SW4 is pressed, decrease MFD (slow down BUSCLK)
  {  
    if (my_mfd) my_mfd--;	  // do not decrease below 0
  }
  ICGC2 = (my_mfd << 4);  	// change MFD factor
}

// A simple software loop delay
void delay(unsigned int value)
{
  for (;value; value--);
}

void main(void) 
{
  unsigned char shift_register = 1;
  SOPT = 0;  // configure SOPT1 register, STOP instruction and COP disabled		
  // ICG configuration:
  // ICG in FEE mode (CLKS = 11), external crystal:
  ICGC1 = ICG_FEE | bREFS | bICG_RANGE;
  ICGC2 = MFDx10;   	      // MFD = 111 (N = 10), RFD = 000 ( R = 1)
  ICGTRM = NVICGTRM;	      // trim IRG oscillator
  while (!ICGS2_DCOS);	    // wait for the DCO to stabilize
  // I/O ports configuration:
  PTDPE = BIT_3 | BIT_2;  	// enable pull ups on PTC7 and PTC5
  PTFDD = 0xFF;	            // PTB0 through PTB7 as outputs
  PTFD = 0;		              // turn off LD401 through LD405
  PTADD = BIT_5;  	        // PTA5 as an output  
  KBI1SC = bKBIE; 	        // turn on KBI2 interrupt
  KBI1PE = BIT_6 | BIT_5; 	// enable KBI inputs corresponding to PTD3 and PTD2
  my_mfd = 3;     	        // initialize "my_mfd" factor
  icg_lols = 0;   	        // initialize "icg_lols" 
  EnableInterrupts;
  while (1)
  {
    PTFD = shift_register;        // change led states
    if (icg_lols) PTFD_PTFD7 = 1; // if icg_lols = 1, turn LED7 on
    delay(30000);	                // delay		
    shift_register <<= 1; 	      // shift one bit to right
    // did we reached the last led?
    if (shift_register==32) shift_register=1;	// restart from the first led
  }
}




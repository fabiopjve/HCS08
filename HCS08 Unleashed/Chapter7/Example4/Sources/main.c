// DEMO9S08LC60 ICG example (FEI mode, BUSCLK = 20 MHz)
// LD401 is connected to PTB3, LD402 is connected to PTB4, LD403 is connected to PTB5
// LD404 is connected to PTB6, LD405 is connected to PTB7
// SW305 is connected to PTC7, SW304 is connected to PTC5
// SW303 is connected to PTC4, SW302 is connected to PTC3

#include <hidef.h> 		  /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "hcs08.h"  		// This is our definition file!

#define LD401 BIT_3 		  // PTB3
#define LD402 BIT_4 		  // PTB4
#define LD403 BIT_5 		  // PTB5
#define LD404 BIT_6 		  // PTB6
#define LD405 BIT_7 		  // PTB7
#define LD406 PTAD_PTAD5	// PTA5

unsigned char my_mfd;

// ICG interrupt (process LOLS and LOCS interrupts)
void interrupt VectorNumber_Vicg icg_isr(void)
{
  ICGS1_ICGIF = 1;  		  // clear ICG interrupt
  // do something ...
  LD406 = 1;			        // turn LD406 on
}

// KBI ISR (process SW305 and SW304 key presses)
void interrupt VectorNumber_Vkeyboard2 kbi2_isr(void)
{
  KBI2SC_KBACK = 1;	      // clear KBI interrupt flag
  if (!PTCD_PTCD7)
  {
    // if SW305 is pressed, increase MFD (speed up BUSCLK)
    my_mfd++;
    if (my_mfd>7) my_mfd = 7;	// limit my_mfd to 7 (N = 18)
  }
  if (!PTCD_PTCD5)
  {
    // if SW304 is pressed, decrease MFD (slow down BUSCLK)
    if (my_mfd) my_mfd--;	// do not decrease below 0
  }
  ICGC2 = my_mfd << 4;  	// change MFD factor
}

// A simple software loop delay
void delay(unsigned int value)
{
  for (;value; value--);
}

void main(void) 
{
  SOPT1 = bBKGDPE;		  // configure SOPT1 register, enable pin BKGD for BDM
  // ICG configuration:
  ICGTRM = NVICGTRM;		// trim IRG oscillator
  ICGC2 = MFDx18;   		// MFD = 111b (N = 18)
  ICGC1 = ICG_FEI;  		// ICG in FEI mode (CLKS = 01b)
  // I/O ports configuration:
  PTCPE = BIT_7 | BIT_5;  	// enable pull ups on PTC7 and PTC5
  PTBDD = 0xFF;			    // PTB0 through PTB7 as outputs
  PTBD = 0;			        // turn off LD401 through LD405
  PTADD = BIT_5;  		  // PTA5 as an output
  // KBI2 configuration:  
  KBI2SC = bKBIE; 		  // turn on KBI2 interrupt
  KBI2PE = BIT_7 | BIT_6; 	// enable KBI inputs corresponding to PTC7 and PTC5
  my_mfd = 7; 			    // initialize "my_mfd" factor 
  EnableInterrupts;		  // clear CCR:I
  while (1)
  {    
    PTBD = LD401; 		// LD401 on
    delay(30000); 		// delay
    PTBD = LD402; 		// LD402 on
    delay(30000); 		// delay
    PTBD = LD403; 		// LD403 on
    delay(30000); 		// delay
    PTBD = LD404; 		// LD404 on
    delay(30000); 		// delay
    PTBD = LD405; 		// LD405 on
    delay(30000); 		// delay
  }
}




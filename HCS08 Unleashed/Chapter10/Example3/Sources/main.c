// DEMO9S08QG8 - ACMP example 3 (object counter)
// LED1 connected to PTB6, LED2 connected to PTB7
// RV1 connected to PTA0 (ACMP+), RZ connected to PTA1 (ACMP-)

#include <hidef.h> 	/* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  	// This is our definition file!

#define LED1  PTBD_PTBD6
#define LED2  PTBD_PTBD7
#define CTIME 300

unsigned int ms_timer, counter, capture;
char flag;

void interrupt VectorNumber_Vtpmch0 timer_isr(void)
{
  TPMC0SC_CH0F = 0;       // clear interrupt flag
  TPMC0V += 1000;         // set the next compare to 1ms ahead
  if (flag) ms_timer++;   // if flag is set, increment ms_timer
}

void interrupt VectorNumber_Vacmp comparator_isr()
{
  ACMPSC_ACF = 1;         // clear interrupt flag
  if (!ACMPSC_ACO)
  {                       // if ACO = 0 (ACMP+ < ACMP-)
    flag = 1;             // set flag (start the ms_timer counter)
    ms_timer = 0;         // clear the ms_timer counter
  } 
  else
  {                       // if ACO = 1 (ACMP+ > ACMP-)
    capture = ms_timer;   // store the current ms_timer count
    flag = 0;             // stop the ms_timer counter
    // if the captured value is greater than CTIME, increment counter
    if (capture>CTIME) counter++;
  }
}

void main(void)
{
  SOPT1 = bBKGDPE;        // Enable debug pin
  ICSSC = NV_FTRIM;       // configure FTRIM value
  ICSTRM = NV_ICSTRM;     // configure TRIM value
  ICSC2 = 0;              // ICSOUT = DCOOUT / 1
  // BUSCLK is now 8 MHz
  PTBD = BIT_7 | BIT_6;   // turn leds off
  PTBDD = BIT_7 | BIT_6;  // PTB6 and PTB7 as outputs
  ACMPSC = bACME | bACIE | ACMOD_3; // Enable ACMP, ACMOD = 3 (both edges)
  TPMSC = TPM_BUSCLK | TPM_DIV8;
  TPMC0SC = bCHIE | TPM_COMPARE_INT;
  TPMC0V = 999;
  counter = 0;
  flag = 0;
  EnableInterrupts;
  while(1)
  {
    if (counter>10) LED1 = 0; // turn LED1 on when counter>10
    if (counter>20) LED2 = 0; // turn LED2 on when counter>20
  }
}

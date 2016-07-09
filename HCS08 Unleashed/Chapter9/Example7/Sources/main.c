// DEMO9S08QG8 MTIM example (finite state machine)
// LED1 cathode is connected to PTB6
// LED2 cathode is connected to PTB7

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define LED1      PTBD_PTBD6
#define LED2      PTBD_PTBD7
#define LED1_TON  200		// LED1 on time = 200ms
#define LED1_TOFF 220		// LED1 off time = 220ms
#define LED2_TON  220		// LED2 on time = 220ms
#define LED2_TOFF 220		// LED2 off time = 220ms

unsigned int timer1, timer2;

enum eled_fsm 
{
  LED_ON, LED_OFF, LED_DELAY
};


// MTIM ISR (one interrupt at every millisecond)
void interrupt VectorNumber_Vmtim mtim_isr(void)
{
  MTIMSC_TOF = 0;       // clear TOF
  if (timer1) timer1--; // if timer1 is greater than zero, decrement it
  if (timer2) timer2--; // if timer2 is greater than zero, decrement it
}




void led1_state_machine(void)
{
  static enum eled_fsm led_state, next_state;
  switch (led_state)
  {
    case LED_ON:
      LED1 = 0;               // turn LED1 on
      timer1 = LED1_TON;      // set timer1 (on delay)
      led_state = LED_DELAY;  // change led state to delay
      next_state = LED_OFF;   // next state is "off"
      break;
    case LED_OFF:
      LED1 = 1;               // turn LED1 off
      timer1 = LED1_TOFF;     // set timer1 (off delay)
      led_state = LED_DELAY;  // change led state to delay
      next_state = LED_ON;    // next state is "on" 
      break;
    case LED_DELAY:           // delay
      // when timer1 is zero, change led state
      if (!timer1) led_state = next_state;
      break;
  }
}

void led2_state_machine(void)
{
  static enum eled_fsm led_state, next_state;
  switch (led_state)
  {
    case LED_ON:
      LED2 = 0;               // turn LED2 on
      timer2 = LED2_TON;      // set timer2 (on delay)
      led_state = LED_DELAY;  // change led state to delay
      next_state = LED_OFF;   // next state is "off"
      break;
    case LED_OFF:
      LED2 = 1;               // turn LED2 off
      timer2 = LED2_TOFF;     // set timer2 (off delay)
      led_state = LED_DELAY;  // change led state to delay
      next_state = LED_ON;    // next state is "on" 
      break;
    case LED_DELAY:           // delay
      // when timer2 is zero, change led state
      if (!timer2) led_state = next_state;
      break;
  }
}

void mcu_init(void)
{
  // configure SOPT1 register, enable the BKGD pin
  SOPT1 = bBKGDPE;
  PTBDD = 0xFF;           // PTB pins as outputs 
  PTBD = 0xFF;            // Set all PTB outputs
  // Configure MTIM: interrupt enabled, MTIM clock = BUSCLK/32, modulo = 125
  // Ttof = 4MHz/(32*125) = 1000
  MTIMSC = bTOIE;
  MTIMCLK = MTIM_DIV32;
  MTIMMOD = 124;          // MTIM modulo = 124+1 = 125
  EnableInterrupts;       // Enable interrupts
}

void main(void) 
{
  mcu_init();
  while (1)
  {
    led1_state_machine();
    led2_state_machine();
  }
}

/*
 A simple electronic speed controller (ESC) for DC motors using the MC9S08QG8
 
 By Fábio Pereira (01/21/08)
 Joinville - SC - Brasil
 www.sctec.com.br

 PTA0 - L298 enable pin (PWM)
 PTA1 - L298 direction input 1 (IN1)
 PTA2 - L298 direction input 2 (IN2)
 PTA3 - PPM input
 PTA4 - debug pin (BKGD)
 PTA5 - reset (not used)
 PTB0 - forward direction led
 PTB1 - backward direction led
 PTB2 - motor stopped led
 PTB3 - <25% throttle
 PTB4 - <50% throttle
 PTB5 - <75% throttle
 PTB6 - full throttle
 PTB7 - not used

 TPM:
  - PWM generation for the L298 (channel 0)
  - Pulse width measurement (KBI interrupt)

*/

#define EN      PTAD_PTAD0
#define IN1     PTAD_PTAD1
#define IN2     PTAD_PTAD2
#define LED_FWD PTBD_PTBD0
#define LED_BWD PTBD_PTBD1
#define LED_STP PTBD_PTBD2
#define LED25   PTBD_PTBD3
#define LED50   PTBD_PTBD4
#define LED75   PTBD_PTBD5
#define LED100  PTBD_PTBD6

#include <hidef.h>
#include "derivative.h"
#include "hcs08.h"

volatile unsigned int pulse_width, average_width;
volatile unsigned char new_sample;
volatile unsigned int new_pwm_cycle;

//const byte NVOPT_INIT @0x0000FFBF = MEM_SECURE;

// TPM overflow ISR
void interrupt VectorNumber_Vtpmovf tpm_overflow_isr(void)
{
  TPMSC_TOF = 0;          // clear interrupt flag
  TPMC0V = new_pwm_cycle; // update PWM cycle
}

// KBI ISR
void interrupt VectorNumber_Vkeyboard kbi_isr (void)
{
  static unsigned int last_tpm;
  unsigned int temp;
  KBISC_KBACK = 1;        // clear interrupt flag
  if (KBIES)              // rising edge ?
  {
    last_tpm = TPMCNT;    // store the TPM counting
    KBIES = 0;            // next edge = falling
  }
  else                    // falling edge ?
  {
    temp = TPMCNT;        
    if (last_tpm>temp)    // if the last TPM counting is higher than the the current one
    { 		      // calculate the pulse width
      pulse_width = (4096 - last_tpm) + temp;
    } 
    else                  // if the last TPM counting is lower than the current one
    { 		      // calculate the pulse width
      pulse_width = temp - last_tpm;
    }
    KBIES = BIT_3;        // next edge = rising
    new_sample = 1;       // set the flag (a new sample is ready)
  }
}

// MCU initialization
void MCU_init(void)
{
  SOPT1 = bBKGDPE;        // enable debug pin
  SOPT2 = 0;              // initialize SOPT2
  PTAD = 0;               
  PTBD = 0;
  PTADD = BIT_2 | BIT_1 | BIT_0;  // PTA0, PTA1 and PTA2 as outputs
  PTBDD = 0xFF;           // all PTB pins as outputs
  // Set the ICS module
  // Trim the internal oscillator
  ICSTRM = NV_ICSTRM;
  ICSSC = NV_FTRIM;
  ICSC2 = 0;
  // CPU = 16MHz, BUSCLK = ICSCLK/2 = 8MHz
  // Configure TPM: TPMCK=BUSCLK/8, overflow interrupt enabled
  TPMSC = TPM_BUSCLK | TPM_DIV8 | bTOIE;
  TPMMOD = 4095;          // TPM modulo = 4096
  // Channel 0 as a PWM output (pin PTA0)
  TPMC0SC = TPM_PWM_HIGH;
  // KBI configuration:
  KBISC = bKBIE;          // KBI interrupt enabled
  KBIPE = BIT_3;          // input 3 (PTA3) as a KBI input
  KBIES = BIT_3;          // rising edge on PTA3 generates an interrupt
  KBISC_KBACK = 1; 
  // Global variables initialization
  new_sample = 0;
  average_width = 0;
  EnableInterrupts;       // enable interrupts (CCR:I=0)
}

void main(void) 
{
  volatile unsigned int sum_width;
  MCU_init();             // initialize the MCU
  sum_width = 12000;      // start the sum at 1.5ms (1500*8 = 12000)
  while (1)
  {
    if (new_sample)       // if a new sample (pulse width) is ready
    {
      // add the new sample to the sum of samples and subtract the average
      sum_width = sum_width + pulse_width - average_width;
      // calculate the new average (the sum divided by 8)
      average_width = sum_width>>3;
      new_sample = 0;     // clear the new_sample flag
    }
    if (average_width > 1600)
    {                     // if the pulse is higher than 1.6ms
      IN1 = 0;            // set the L298 direction
      IN2 = 1;
      LED_FWD = 1;        // turn on the FORWARD LED
      LED_BWD = 0;
      LED_STP = 0;
      //if the pulse width is higher than 1.98ms, set the PWM duty cycle to 100%
      if (average_width > 1980) new_pwm_cycle = 4096;
      // calculate the PWM duty cycle (proportional to the pulse width)
      else new_pwm_cycle = (average_width-1600) * 10;
    } 
    else if (average_width < 1400)
    {                     // if the pulse is lower than 1.4ms
      IN1 = 1;            // set the L298 direction
      IN2 = 0;
      LED_FWD = 0;        
      LED_BWD = 1;        // turn on the BACKWARD LED
      LED_STP = 0;
      //if the pulse width is lower than 1.02ms, set the PWM duty cycle to 100%      
      if (average_width < 1020) new_pwm_cycle = 4096;
      // calculate the PWM duty cycle (proportional to the pulse width)
      else new_pwm_cycle = (400-(average_width-1000)) * 10;
    } 
    else    
    {                     // if the pulse is >= 1.4ms and <= 1.6ms
      IN1 = 1;            // stop the motor
      IN2 = 1;
      LED_FWD = 0;
      LED_BWD = 0;
      LED_STP = 1;        // turn on the STOP LED
      new_pwm_cycle = 4096; // set the PWM duty cycle to 100% (brake the motor)
    }
    if (!LED_STP)         // if the STOP LED is not lit
    {
      // turn on the LEDs according to the PWM duty cycle
      if (new_pwm_cycle>3072) LED100 = 1; else LED100 = 0;
      if (new_pwm_cycle>2048) LED75 = 1; else LED75 = 0;
      if (new_pwm_cycle>1024) LED50 = 1; else LED50 = 0;
      if (new_pwm_cycle>64) LED25 = 1; else LED25 = 0;
    }
  }
}

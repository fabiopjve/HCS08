// DEMO9S08QE128 - TPM example (sound generation)
// TPM1 channel 1 pin (PTB5) connected to a piezoelectric buzzer (mounted in the
// demonstration board)

#include <hidef.h>        /* for EnableInterrupts macro */
#include "derivative.h"   /* include peripheral declarations */
#include "hcs08.h"        // This is our header file!

unsigned int channel_reload_value, duration_timer;
char sound_playing;

// TPM1 overflow interrupt isr: control the duration of the sound
void interrupt VectorNumber_Vtpm1ch0 tpm1ch0_isr(void)
{
  TPM1C0SC_CH0F = 0;                    // clear interrupt flag
  TPM1C0V += 4000;                      // next compare in 1 ms
  if (duration_timer) duration_timer--; // decrement the timer if > 0
  else                                  // if the timer is = 0
  {
    TPM1C1SC = 0;                       // disable TPM1CH1 channel (stops sound)
    sound_playing = 0;                  // clear the flag (no sound is playing)
  }
}

// TPM1 channel1 compare interrupt isr: control the frequency generation
// the channel pin is automatically toggled on each compare
void interrupt VectorNumber_Vtpm1ch1 tpm1ch1_compare(void)
{
  TPM1C1SC_CH1F = 0;                    // clear interrupt flag
  TPM1C1V += channel_reload_value;      // next compare
}

// Outputs a sound through the TPM1CH1 pin (PTB5): freq in Hz, dur in ms
void sound(unsigned int freq, unsigned int dur)
{
  while (sound_playing);      	        // if a sound is playing, wait until if finishes
  // the channel reload value is equal to half the period of the signal
  channel_reload_value = (4000000/freq)/2;
  duration_timer = dur;       	        // set the total duration time
  // set channel 1 to compare mode (toggling the channel pin)
  TPM1C1V = channel_reload_value; 	    // set the first compare value  
  TPM1C1SC = bCHIE | TPM_COMPARE_TOGGLE;
  sound_playing = 1;          	        // sound is playing (until the flag is cleared)
}

void main(void)
{
  unsigned char song_sel;
  SOPT1 = bBKGDPE;        	      	// enable the debug pin
  ICSSC = DCO_MID | NVFTRIM;	    	// configure FTRIM value, select DCO high range
  ICSTRM = NVICSTRM;			          // configure TRIM value
  ICSC1 = ICS_FLL | bIREFS;		      // select FEI mode (ICSOUT = DCOOUT = 1024 * IRCLK)
  ICSC2 = BDIV_1;                 	// ICSOUT = DCOOUT / 1
  // BUSCLK = 16MHz
  TPM1SC = TPM_BUSCLK | TPM_DIV4; 	// TPMCK = 4MHz
  // set channel 0 to compare mode (interrupt only)
  TPM1C0V = 3999;
  TPM1C0SC = bCHIE | TPM_COMPARE_INT;   
  sound_playing = 0;  
  EnableInterrupts;	  		// enable interrupts (CCR:I=0)
  while(1)
  {
    sound(1000,300);			// output a 1kHz sound for 300ms
    sound(2000,500);			// output a 2kHz sound for 500ms
  }
}

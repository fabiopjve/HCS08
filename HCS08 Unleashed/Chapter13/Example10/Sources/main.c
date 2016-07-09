/*
  DEMO9S08QE128 - Music-shake - RTTTL player with accelerometer sensing 

  By Fábio Pereira (01/20/08)
  Joinville - SC - Brasil
  www.sctec.com.br
 	
	PTA1 - XOUT
	PTA6 - YOUT
	PTA7 - ZOUT
	PTC4 - MMA7260 sleep mode selection (0 = sleep, 1 = operating)
	PTD0 - MMA7260 g-select1 input
	PTD1 - MMA7260 g-select2 input
	PTB5 - piezoelectric buzzer (TPM1CH1 pin)

 	ADC:
	- accelerometer measurements (PTA1, PTA6 and PTA7)
	TPM1:
	- channel 0: controls the sound duration and accelerometer sampling
	- channel 1: frequency generation

*/



#include <hidef.h>        /* for EnableInterrupts macro */
#include "derivative.h"   /* include peripheral declarations */
#include "hcs08.h"        // This is our header file!

#define ACCEL_ON  PTCD_PTCD4

// RTTL music collection
const char *rtttl_library[]=
{
{"U2Newyears:d=8,o=6,b=125:a5,a5,c,4a5,a5,a5,a5,c,c,e,4c,c,c,e,e,e,g,4e,e,e,a5,e,e,g,e,e,e,e,e,a5,a5,c,a5,a5,a5,a5,c,c,e,c,c,c,e,e,e,g,e,e,e,2a5"},
{"MissionImp:d=4,o=6,b=150:16d5,16d#5,16d5,16d#5,16d5,16d#5,16d5,16d5,16d#5,16e5,16f5,16f#5,16g5,8g5,4p,8g5,4p,8a#5,8p,8c6,8p,8g5,4p,8g5,4p,8f5,8p,8p,8g5,4p,4p,8a#5,8p,8c6,8p,8g5,4p,4p,8f5,8p,8f#5,8p,8a#5,8g5,1d5"},
{"ET:d=2,o=6,b=200:d,a,8g,8f#,8e,8f#,d,1a5,b5,b,8a,8g#,8f#,8g#,e,1c#7,e,d7,8c#7,8b,8a,8g,f,d.,16d,16c#,16d,16e,f,d,d7,1c#7"},
{"Batman:d=8,o=5,b=160:16a,16g#,16g,16f#,16f,16f#,16g,16g#,4a.,p,d,d,c#,c#,c,c,c#,c#,d,d,c#,c#,c,c,c#,c#,d,d,c#,c#,c,c,c#,c#,g6,p,4g6"},
{"Axelf:d=8,o=5,b=160:4f#,a.,f#,16f#,a#,f#,e,4f#,c6.,f#,16f#,d6,c#6,a,f#,c#6,f#6,16f#,e,16e,c#,g#,4f#."},
{"Hogans:d=16,o=6,b=45:f5.,g#5.,c#.,f.,f#,32g#,32f#.,32f.,8d#.,f#,32g#,32f#.,32f.,d#.,g#5.,c#,32c,32c#.,32a#5.,8g#5.,f5.,g#5.,c#.,f5.,32f#5.,a#5.,32f#5.,d#.,f#.,32f.,g#.,32f.,c#.,d#.,8c#."},
{"Jamesbond:d=4,o=5,b=320:c,8d,8d,d,2d,c,c,c,c,8d#,8d#,2d#,d,d,d,c,8d,8d,d,2d,c,c,c,c,8d#,8d#,d#,2d#,d,c#,c,c6,1b.,g,f,1g."},
{"Pinkpanther:d=16,o=5,b=160:8d#,8e,2p,8f#,8g,2p,8d#,8e,p,8f#,8g,p,8c6,8b,p,8d#,8e,p,8b,2a#,2p,a,g,e,d,2e"},
{"Countdown:d=4,o=5,b=125:p,8p,16b,16a,b,e,p,8p,16c6,16b,8c6,8b,a,p,8p,16c6,16b,c6,e,p,8p,16a,16g,8a,8g,8f#,8a,g.,16f#,16g,a.,16g,16a,8b,8a,8g,8f#,e,c6,2b.,16b,16c6,16b,16a,1b"},
{"Adamsfamily:d=4,o=5,b=160:8c,f,8a,f,8c,b4,2g,8f,e,8g,e,8e4,a4,2f,8c,f,8a,f,8c,b4,2g,8f,e,8c,d,8e,1f,8c,8d,8e,8f,1p,8d,8e,8f#,8g,1p,8d,8e,8f#,8g,p,8d,8e,8f#,8g,p,8c,8d,8e,8f"},
{"TheSimpsons:d=4,o=5,b=160:c.6,e6,f#6,8a6,g.6,e6,c6,8a,8f#,8f#,8f#,2g,8p,8p,8f#,8f#,8f#,8g,a#.,8c6,8c6,8c6,c6"},
{"Indiana:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6"},
{"GirlFromIpanema:d=4,o=5,b=160:g.,8e,8e,d,g.,8e,e,8e,8d,g.,e,e,8d,g,8g,8e,e,8e,8d,f,d,d,8d,8c,e,c,c,8c,a#4,2c"},
{"TakeOnMe:d=4,o=4,b=160:8f#5,8f#5,8f#5,8d5,8p,8b,8p,8e5,8p,8e5,8p,8e5,8g#5,8g#5,8a5,8b5,8a5,8a5,8a5,8e5,8p,8d5,8p,8f#5,8p,8f#5,8p,8f#5,8e5,8e5,8f#5,8e5,8f#5,8f#5,8f#5,8d5,8p,8b,8p,8e5,8p,8e5,8p,8e5,8g#5,8g#5,8a5,8b5,8a5,8a5,8a5,8e5,8p,8d5,8p,8f#5,8p,8f#5,8p,8f#5,8e5,8e5"},
{"She:d=16,o=5,b=63:32b,4c6,c6,c6,8b,8d6,8c6.,8b,a,32b,2c6,p,c6,c6,b,8d6,8c6,8b,a,4c6.,8c6,b,32c6,4d6,8c6.,b,8a,8g,8f.,e,f,e,32f,2g,8p"},
{"Barbiegirl:d=4,o=5,b=125:8g#,8e,8g#,8c#6,a,p,8f#,8d#,8f#,8b,g#,8f#,8e,p,8e,8c#,f#,c#,p,8f#,8e,g#,f#"},
{"Entertainer:d=4,o=5,b=140:8d,8d#,8e,c6,8e,c6,8e,2c.6,8c6,8d6,8d#6,8e6,8c6,8d6,e6,8b,d6,2c6,p,8d,8d#,8e,c6,8e,c6,8e,2c.6,8p,8a,8g,8f#,8a,8c6,e6,8d6,8c6,8a,2d6"},
{"Xfiles:d=4,o=5,b=125:e,b,a,b,d6,2b.,1p,e,b,a,b,e6,2b.,1p,g6,f#6,e6,d6,e6,2b.,1p,g6,f#6,e6,d6,f#6,2b.,1p,e,b,a,b,d6,2b.,1p,e,b,a,b,e6,2b.,1p,e6,2b."},
{"Autumn:d=8,o=6,b=125:a,a,a,a#,4a,a,a#,a,a,a,a#,4a,a,a#,a,16g,16a,a#,a,g.,16p,a,a,a,a#,4a,a,a#,a,a,a,a#,4a,a,a#,a,16g,16a,a#,a,g."},
{"Spring:d=16,o=6,b=125:8e,8g#,8g#,8g#,f#,e,4b.,b,a,8g#,8g#,8g#,f#,e,4b.,b,a,8g#,a,b,8a,8g#,8f#,8d#,4b.,8e,8g#,8g#,8g#,f#,e,4b.,b,a,8g#,8g#,8g#,f#,e,4b.,b,a,8g#,a,b,8a,8g#,8f#,8d#,4b."},
};

// The following array stores the frequencies for the musical notes
const unsigned int note[4][12] =
{// C    C#    D     D#    E     F     F#    G     G#    A     A#    B
   262,  277,  294,  311,  330,  349,  370,  392,  415,  440,  466,  494, // 4th octave
   523,  554,  587,  622,  659,  698,  740,  784,  830,  880,  932,  988, // 5th octave
  1047, 1109, 1175, 1244, 1319, 1397, 1480, 1568, 1660, 1760, 1865, 1976, // 6th octave
  2093, 2218, 2349, 2489, 2637, 2794, 2960, 3136, 3320, 3520, 3728, 3951  // 7th octave
};

unsigned int channel_reload_value, duration_timer;
unsigned char accel_timer;
char sound_playing=0;
char duration, octave;
unsigned int tempo;
unsigned int y_axis, y_avg;
char next_song_flag = 0;
unsigned int last_y;

// TPM1 overflow ISR: controls the duration of the sound
void interrupt VectorNumber_Vtpm1ch0 tpm1ch0_isr(void)
{
  TPM1C0SC_CH0F = 0;                    	// clear interrupt flag
  TPM1C0V += 4000;                      	// next compare in 1 ms
  if (duration_timer) duration_timer--; 	// decrement the timer if > 0
  else                                  	// if the timer is = 0
  {
    TPM1C1SC = 0;                       	// disable TPM1CH1 channel (stops sound)
    sound_playing = 0;                  	// clear the flag (no sound is not playing)
  }
  if (accel_timer) accel_timer--;       	// acceleration timer
  else
  {
    last_y = y_axis;	// sample the y axis
    accel_timer = 50;	// next sample in 50ms
  }
}


// TPM1 channel1 compare ISR: control the frequency generation
// the channel pin is automatically toggled on each compare
void interrupt VectorNumber_Vtpm1ch1 tpm1ch1_compare(void)
{
  TPM1C1SC_CH1F = 0;                    // clear interrupt flag
  TPM1C1V += channel_reload_value;      // next compare
}

// Output a sound through the TPM1CH1 pin (PTB5): freq in Hz, dur in ms
void sound(unsigned int freq, unsigned int dur)
{
  while (sound_playing);      // if a sound is playing, waits until if finishes
  // the channel reload value is equal to half the period of the signal
  channel_reload_value = (4000000/freq)/2;
  duration_timer = dur;       // set the total duration time
  // set channel 1 to compare mode (toggling the channel pin)
  TPM1C1V = channel_reload_value; // set the first compare value  
  TPM1C1SC = bCHIE | TPM_COMPARE_TOGGLE;
  sound_playing = 1;          // sound is playing (until the flag is cleared)
}


// This function plays a song passed in a string in RTTTL format
void play_song(char *song)
{
  unsigned char temp_duration, temp_octave, current_note, dot_flag;
  unsigned int calc_duration;
  duration = 4;                 // standard duration = 4/4 = 1 beat
  tempo = 63;                   // standard tempo = 63 bpm
  octave = 6;                   // standard octave = 6th
  while (*song != ':') song++;  // find the first ':'
  song++;                       // skip the ':'
  while (*song!=':')            // repeat until find a ':'
  {
    if (*song == 'd')           // if it is the duration setting
    {
      duration = 0;             // set duration to zero (temporarily)
      song++;                   // advance to the next character
      while (*song == '=') song++;  // skip the '='
      while (*song == ' ') song++;  // skip the spaces
      // if the character is a number, set the duration
      if (*song>='0' && *song<='9') duration = *song - '0';
      song++;                   // advance to the next character
      // if the character is also a number (duration can be 2 digits long)
      if (*song>='0' && *song<='9')
      { // multiply duration by 10 and add the value of the character
        duration = duration*10 + (*song - '0');
        song++;                 // advance to the next character
      }
      while (*song == ',') song++;  // skip the ','
    }
    if (*song == 'o')           // if it is the octave setting
    {
      octave = 0;               // set octave to zero (temporarily)
      song++;                   // advance to the next character
      while (*song == '=') song++;  // skip the '='
      while (*song == ' ') song++;  // skip the spaces
      // if the character is a number, then set the octave
      if (*song>='0' && *song<='9') octave = *song - '0';
      song++;                   // advance to the next character
      while (*song == ',') song++;  // skips the ','
    }
    if (*song == 'b')           // if it is the tempo setting (beats per minute)
    {
      tempo = 0;                // set tempo to zero (temporarily)
      song++;                   // advance to the next character
      while (*song == '=') song++;  // skips '='
      while (*song == ' ') song++;  // skips spaces
      // now read the tempo setting (can be 3 digits long)
      if (*song>='0' && *song<='9') tempo = *song - '0';
      song++;                   // advance to the next character
      if (*song>='0' && *song<='9')
      {
        tempo = tempo*10 + (*song - '0'); // tempo is two digits
        song++;                 // advance to the next character
        if (*song>='0' && *song<='9')
        {
          tempo = tempo*10 + (*song - '0'); // tempo is three digits
          song++;               // advance to the next character
        }
      }
      while (*song == ',') song++;  // skip the ','
    }    
    while (*song == ',') song++;    // skip the ','    
  }
  song++;                       // advance to the next character
  // read the musical notes
  while (*song)                 // repeat until the character is null
  {
    current_note = 255;         // default note = pause
    temp_octave = octave;       // set the octave to the music default
    temp_duration = duration;   // set the duration to the music default
    dot_flag = 0;               // clear the dot detection flag
    // look for a duration prefix
    if (*song>='0' && *song<='9')
    {
      temp_duration = *song - '0';
      song++;
      if (*song>='0' && *song<='9')
      {
        temp_duration = temp_duration*10 + (*song - '0');
        song++;
      }
    }
    // look for a note
    switch (*song)
    {
      case 'c': current_note = 0; break;    // C (do)
      case 'd': current_note = 2; break;    // D (re)
      case 'e': current_note = 4; break;    // E (mi)
      case 'f': current_note = 5; break;    // F (fa)
      case 'g': current_note = 7; break;    // G (sol)
      case 'a': current_note = 9; break;    // A (la)
      case 'b': current_note = 11; break;   // B (si)
      case 'p': current_note = 255; break;  // pause
    }
    song++;                     // advance to the next character
    // look for a # following the note
    if (*song=='#')
    {
      current_note++;   // increment the note (A->A#, C->C#, D->D#, F->F#, G->G#)
      song++;                   // advance to the next character
    }
    // look for a '.' (extend the note duration in 50%)
    if (*song=='.')
    {
      dot_flag = 1;             // if a '.' is found, set the flag 
      song++;                   // advance to the next character
    }


    // look for an octave postfix    
    if (*song>='0' && *song<='9')
    {
      temp_octave = *song - '0';// the temporary octave is set accordingly
      song++;                   // advance to the next character
    }
    if (*song=='.')             // a dot can also be found after the octave (???)
    {
      dot_flag = 1;             // if a '.' is found, set the flag 
      song++;                   // advance to the next character
    }
    while (*song == ',') song++;    // skip the ','
    // calculate the note duration
    calc_duration = (60000/tempo)/(temp_duration);
    calc_duration *= 4;         // a whole note has four beats
    // check if the dot flag is set, if it is set, extend the duration in 50%
    if (dot_flag) calc_duration = (calc_duration*3)/2;
    // if the current note is not a pause, play the note using the sound function
    if (current_note<255) sound(note[temp_octave-4][current_note],calc_duration);
    else
    { // if the current note = 255 (pause), just wait for the specified amount of time
      duration_timer = calc_duration;
      sound_playing = 1;        
    }
    while (sound_playing);      // wait until the current note/pause has finished
    if (next_song_flag) break;  // if a shake was detected, skip to the next song
  }
}

// ADC ISR
void interrupt VectorNumber_Vadc adc_isr(void)
{
  y_avg = (y_avg + ADCR) - y_axis;	// average the accelerometer reading
  y_axis = y_avg >> 3;		// current Y axis value is y_avg/8
  // if an acceleration towards +Y is detected, followed by an acceleration towards -Y
  // skip to the next song
  if (y_axis<800 && last_y>3000) next_song_flag = 1;
}

void main(void)
{
  unsigned char song_sel;
  SOPT1 = bBKGDPE;        	      	// enable the debug pin
  ICSSC = DCO_MID | NVFTRIM;	      	// configure FTRIM value, select DCO high range
  ICSTRM = NVICSTRM;		     	// configure TRIM value
  ICSC1 = ICS_FLL | bIREFS;       	// select FEI mode (ICSOUT = DCOOUT = 1024 * IRCLK)
  ICSC2 = BDIV_1;                 	// ICSOUT = DCOOUT / 1
  // BUSCLK = 16MHz
  TPM1SC = TPM_BUSCLK | TPM_DIV4; 	// TPMCK = 4MHz
  // set channel 0 to compare mode (interrupt only), 1ms interrupts
  TPM1C0V = 3999;
  TPM1C0SC = bCHIE | TPM_COMPARE_INT;
  // enable long sampling, 12-bit mode, ADICLK = 11b, ADCK = ADCLK/8
  // ADC sampling rate = 25ksps
  ADCCFG = bADLSMP | ADC_12BITS | ADC_INTCLK | ADIV_8;
  APCTL2 = bADPC8;                 	// ADP8 in analog mode
  // ADC channel 8, interrupts enabled, continuous conversion
  ADCSC1 = bAIEN | ADC_CH8 | bADCO;
  PTDD = 0;                       	// select 1.5g sensitivity
  PTDDD = BIT_0 | BIT_1; 		// PTD0 and PTD1 as outputs
  PTBDS = BIT_5;			// turn on drive strengh on PTB5 (buzzer output)
  PTCDD = BIT_4;                  	// PTC4 as an output
  y_avg = 0;
  y_axis = 0;
  accel_timer = 50;               	// force sampling
  ACCEL_ON = 1;          		// turn on the accelerometer (sleep mode off)
  EnableInterrupts;		       	// enable interrupts (CCR:I=0)
  while(1)
  {
    for (song_sel=0;song_sel<20;song_sel++)
    {
      duration_timer = 250;
      sound_playing = 1;
      while (sound_playing);		// wait for 250ms before starting the next song      
      play_song(rtttl_library[song_sel]); // play the song


      // wait for 2 seconds before start the next song
      duration_timer = 2000;		// wait for 2000ms before continuing
      sound_playing = 1;
      // skip to the next song if the duration timer times out or the song is skipped
      while (sound_playing && !next_song_flag);
      next_song_flag = 0;
    }
  }
}

// DEMO9S08QG8 - SPI example 3 (bit-banged  SPI)
// DATA connected to PTB3/MOSI
// CLOCK connected to PTB2/SPSCK
// STROBE connected to PTB5/SS

#include <hidef.h> 	      /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define DATA    PTBD_PTBD3
#define CLOCK   PTBD_PTBD2
#define STROBE  PTBD_PTBD5

char counter;

// MCU initialization
void mcu_init(void)
{
  SOPT1 = bBKGDPE;        		// Enable debug pin
  ICSSC = NV_FTRIM;       		// configure FTRIM value
  ICSTRM = NV_ICSTRM;     		// configure TRIM value
  // BUSCLK is now 4 MHz
  // Configure Port B pins:
  PTBDD = BIT_2 | BIT_3 | BIT_5;  	// PTB2, PTB3 and PTB5 as outputs
  EnableInterrupts;
}

// This is the bit-banged SPI function. It emulates SPI master (output only) in SPI mode 0
void spi_write(char data)
{
  unsigned temp;
  STROBE = 0;             	// enable the 4094
  for (temp=8; temp; temp--)  	// repeat eight times
  { // if the msb of data = 1, force the DATA pin to 1, else force the DATA pin to 0
    if (data & BIT_7) DATA = 1; else DATA = 0;
    CLOCK = 1;            	// set the CLOCK pin
    data = data << 1;     	// shift data one bit to the left
    CLOCK = 0;            	// clear the CLOCK pin
  }
  STROBE = 1;             	// disable the 4094
}

void main(void)
{
  unsigned int temp;
  mcu_init();             	// initialize the MCU
  counter = 0;
  while(1)
  {
    spi_write(counter++); 	// write to the 4094
    for (temp=10000; temp; temp--); 	// wait for a while
  }
}

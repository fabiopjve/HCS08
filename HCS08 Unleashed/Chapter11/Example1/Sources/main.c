// DEMO9S08QG8 - SPI example 1 (driving a 4094 shift register)
// STROBE connected to PTB5/SS
// DATA connected to PTB3/MOSI
// CLOCK connected to PTB2/SPSCK

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

char counter;

void mcu_init(void)
{
  SOPT1 = bBKGDPE;        	// Enable debug pin
  ICSSC = NV_FTRIM;       	// configure FTRIM value
  ICSTRM = NV_ICSTRM;     	// configure TRIM value
  // BUSCLK is now 8 MHz
  // Now we configure the SPI interface: Master mode, SS pin as slave select output
  // SPI clock = BUSCLK/4 = 1MHz
  SPIC1 = bSPE | bMSTR | bSSOE;
  SPIC2 = bMODFEN;
  SPIBR = SPI_PRE1 | SPI_DIV4;  // SPI clock = BUSCLK/4 = 1MHz
}

void main(void)
{
  unsigned int temp;
  mcu_init();             	// initialize the MCU
  counter = 0;
  while(1)
  {
    temp = SPIS;          	// read the SPI flags
    SPID = counter++;     	// write "counter" through the SPI
    for (temp=20000; temp; temp--); // wait for a while
  }
}



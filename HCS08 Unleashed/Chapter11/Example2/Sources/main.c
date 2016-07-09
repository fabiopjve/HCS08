// DEMO9S08QG8 - SPI example 2 (driving two 4094 shift registers)
// DATA connected to PTB3/MOSI
// CLOCK connected to PTB2/SPSCK
// STROBE (IC2) connected to PTB5/SS
// STROBE (IC3) connected to PTB6

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define SS1 PTBD_PTBD5
#define SS2 PTBD_PTBD6

char counter;
char spi_busy;

void interrupt VectorNumber_Vspi spi_receive_isr(void)
{
  unsigned char temp;
  temp = SPIS;    // read SPIS
  temp = SPID;    // read received data (dummy read, clear SPRF)
  SS1 = 1;        // disable SS1 (IC2)
  SS2 = 1;        // disable SS2 (IC3)
  spi_busy = 0;   // clear busy flag (SPI is idle)
}

// MCU initialization
void mcu_init(void)
{
  SOPT1 = bBKGDPE;        // Enable debug pin
  ICSSC = NV_FTRIM;       // configure FTRIM value
  ICSTRM = NV_ICSTRM;     // configure TRIM value
  // BUSCLK is now 4 MHz
  // Now we configure the SPI interface:
  // Master mode, SS pin as slave select output, SPI clock = BUSCLK/4 = 1MHz
  SPIC1 = bSPIE | bSPE | bMSTR;
  SPIBR = SPI_PRE1 | SPI_DIV4;  	// SPI clock = BUSCLK/4 = 1MHz
  // Configure Port B pins:
  PTBDD = BIT_5 | BIT_6;  		// PTB5 and PTB6 as outputs
  EnableInterrupts;
}

void spi_write(char data, char device)
{
  char temp;
  while (spi_busy);     // verify if the SPI is idle (wait until it is free)
  spi_busy = 1;         // SPI is busy from now on
  temp = SPIS;          // read the SPI flags
  if (device) SS2 = 0; else SS1 = 0;  // enable the desired 4094
  SPID = data;          // transmit data through SPI
}

void main(void)
{
  unsigned int temp;
  mcu_init();             // initialize the MCU
  counter = 0;
  while(1)
  {
    spi_write(counter++,0);		// write to the first 4094 (IC2)
    spi_write(~counter,1);		// write to the second 4094 (IC3)
    for (temp=20000; temp; temp--);	// wait for a while
  }
}

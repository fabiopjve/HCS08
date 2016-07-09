// DEMO9S08QG8 - I2C example 1 (I2C EEPROM (no interrupts))
// EEPROM SDA pin connected to PTA2/SDA
// EEPROM SCL pin connected to PTA3/SCL

#include <hidef.h> 	      /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  	    // This is our definition file!

#define EEPROM_ID   0xA0
#define EEPROM_ADR  0
#define RD          1
#define WR          0

unsigned int eeprom_address;
char eeprom_data, operation;


// MCU initialization
void mcu_init(void)
{
  SOPT1 = bBKGDPE;        	// Enable debug pin
  ICSSC = NV_FTRIM;       	// configure FTRIM value
  ICSTRM = NV_ICSTRM;     	// configure TRIM value
  // BUSCLK is now 4 MHz
  // Now we configure the I2C interface:
  // I2C clock is 4MHz*1/40 = 100kHz, SDA hold = 9/4MHz=2.25us
  IICF = I2C_M1 | 0x0B;   
  IICC = bIICEN;          	// enable the I2C interface
}


// This is the I2C EEPROM byte writing function
void eeprom_bytewrite(unsigned int address, char data)
{
  char temp;
  do  // first we send the address field
  {
    // enter master transmit mode and send a start
    IICC = bIICEN | bTX | bMST;
    // send the address field (R/W = write)
    IICD = EEPROM_ID | EEPROM_ADR | WR;
    for (temp=40; temp; temp--);  	// wait for a while
    while (!IICS_TCF);            	// wait until the transmission ends


    if (IICS_RXAK)                	// was an ACK received from the memory ?
    {
      IICC = bIICEN;              	// if no ACK was received, send a stop
      for (temp=40; temp; temp--);	// wait for a while
    }
  } while (IICS_RXAK); 	// if no ACK was received, resend the address field
  IICD = address >> 8;    	// send the higher byte of the address
  while (!IICS_TCF);      	// wait until the transmission ends
  IICD = address & 0xFF;  	// send the lower byte of the address
  while (!IICS_TCF);      	// wait until the transmission ends
  IICD = data;            	// send the data to be written
  for (temp=40; temp; temp--);	// wait for a while
  while (!IICS_TCF);      	// wait until the transmission ends
  // send stop, exit master mode (the memory starts programming)
  IICC = bIICEN;          
}

// This is the I2C EEPROM byte reading function
char eeprom_byteread(unsigned int address)
{
  char temp, data;
  while (IICS_BUSY);
  IICS_ARBL = 1;          	// clear ARBL flag
  IICC = bIICEN | bTX | bMST;	// enter master transmitter mode
  IICD = EEPROM_ID | EEPROM_ADR | WR;
  for (temp=40; temp; temp--);	// wait for a while
  while (!IICS_TCF);      	// wait until the transmission ends
  IICD = address >> 8;    	// send the higher byte of the address
  while (!IICS_TCF);      	// wait until the transmission ends
  IICD = address & 0xFF;  	// send the lower byte of the address
  while (!IICS_TCF);      	// wait until the transmission ends
  IICC_RSTA = 1;          	// send a repeated start
  IICD = EEPROM_ID | EEPROM_ADR | RD; // read operation
  for (temp=40; temp; temp--);	// wait for a while
  while (!IICS_TCF);      	// wait until the transmission ends
  IICC_TX = 0;            	// receive mode
  data = IICD;            	// dummy read of IICD (start the reception)
  for (temp=40; temp; temp--);	// wait for a while
  while (!IICS_TCF);      	// wait until the transmission ends
  IICC_TXAK = 1;          	// send NACK in the next read
  data = IICD;            	// read data from EEPROM
  while (!IICS_TCF);      	// wait until the transmission ends
  IICC = bIICEN;          	// send a stop, leave master mode
  for (temp=100; temp; temp--);	// wait for a while
  return(data);
}

void main(void)
{
  mcu_init();             		// initialize the MCU
  eeprom_address = 0;
  eeprom_data = 0;
  operation = 0;
  while(1)
  {
    if (operation==1)     		// write operation
    {
      eeprom_bytewrite(eeprom_address, eeprom_data);
      operation = 0;
    }
    if (operation==2)     		// read operation
    {
      eeprom_data = eeprom_byteread(eeprom_address);
      operation = 0;
    }
  }
}

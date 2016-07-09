/* 
   DEMO9S08QG8 - I2C slave ADC converter and I/O expander
   SDA pin connected to PTA2/SDA
   SCL pin connected to PTA3/SCL
   Registers:
   0 - channel selection
   1 - higher byte of ADC conversion
   2 - lower byte of ADC conversion
   3 - I/O direction
   4 - output register
   5 - input register
*/

#include <hidef.h> 		      /* for EnableInterrupts macro */
#include "derivative.h" 		/* include peripheral declarations */
#include "hcs08.h"  		    // This is our definition file!

#define ADC_ID      0x90		// I2C slave device ID
#define ADC_ADR     0		    // I2C slave address

#define ADCH	      0		    // ADC channel selection register
#define ADCR_HIGH   1		    // ADC result (high byte)
#define ADCR_LOW    2		    // ADC result (low byte)
#define IO_DIR      3		    // I/O direction selection register
#define IO_OUT      4		    // I/O output register
#define IO_IN       5		    // I/O input register
#define IO_PUE      6		    // I/O pull up enable register

char registers[7], current_reg, block;

//const byte NVOPT_INIT @0x0000FFBF = MEM_SECURE;

// MCU initialization
void MCU_init(void)
{
  SOPT1 = bBKGDPE;        	// Enable debug pin
  ICSSC = NV_FTRIM;       	// configure FTRIM value
  ICSTRM = NV_ICSTRM;     	// configure TRIM value
  // BUSCLK is 4 MHz
  // Now we configure the I2C interface:
  // I2C clock is 4MHz*1/40 = 100kHz, SDA hold = 9/4MHz=2.25us
  IICF = I2C_M1 | 0x0B;
  IICA = ADC_ID | ADC_ADR;	// set the slave own address
  IICC = bIICEN | bIICIE; 	// enable the I2C interface
  // Configure the ADC
  ADCCFG = ADIV_8 | bADLSMP | ADC_10BITS;
  ADCSC1 = bADCO | ADCH0;
  EnableInterrupts;
}

// This is the I2C interrupt servicing function
void interrupt VectorNumber_Viic iic_isr(void)
{
  char temp;
  static char rx_counter;
  // verify the source of I2C interrupt:
  if (IICS_IAAS)          	// slave device addressed
  {
    if (IICS_SRW)
    {			// slave transmits data to master 
      IICC_TX = 1;        	// set transmit mode
      if (current_reg<7) temp = registers[current_reg];
      else temp = 0;
      IICD = temp;
      if (current_reg==ADCR_HIGH) block = 1;
      if (current_reg==ADCR_LOW) block = 0;
    }
    else
    {			// slave receives data from master
      IICC_TX = 0;      		// go to receive mode
      temp = IICD;      		// dummy read of IICD
      rx_counter = 0;
    }
  } 
  else if (IICS_TCF)      	// a transfer was completed
  {
    if (IICC_TX)
    { 			// TX=1 -> transmit mode
      if (IICS_RXAK)
      { 			// NACK received from the master receiver
        IICC_TX = 0;      	// go to receive mode
        temp = IICD;      	// dummy read of IICD
      }
      else
      { 			// ACK received from the master receiver
        if (current_reg<7) temp = registers[current_reg];
        else temp = 0;
        // send the current register content (or 0x00 if no valid register is selected)
        IICD = temp;
      }
    }
    else
    { 			// TX=0 -> receive mode
      if (!rx_counter)
      { 			// if it is the first byte
        IICC_TXAK = 0;    	// send an ACK
        current_reg = IICD;	// select an internal register
      }
      else
      { 			// if it is the second byte
        IICC_TXAK = 1;    	// send a NACK
        temp = IICD;		// read data from the I2C bus
        if (!current_reg)
        { // if the current register is 0x00 (ADCH), config the ADC channel
          if (registers[ADCH]) ADCSC1 = bADCO | ADCH1; 
          else ADCSC1 = bADCO | ADCH0;       
        }
        // if the current register is valid (<7), update the register content
        else if (current_reg<7) registers[current_reg] = temp;
      }
      rx_counter++;
    }
  }
  IICS_IICIF = 1;         	// clear the interrupt flag
}



void main(void)
{
  MCU_init();             	// initialize the MCU
  for (current_reg=0; current_reg<7; current_reg++) registers[current_reg] = 0;
  current_reg = 0;		// set the current register to ADCH
  while(1)
  {
    if (ADCSC1_COCO && !block)	
    { // if an ADC conversion is ready and the ADC is not blocked
      registers[ADCR_HIGH] = ADCRH;	// update the ADCR_HIGH register
      registers[ADCR_LOW] = ADCRL;	// update the ADCR_LOW register
    }
    PTBDD = registers[IO_DIR];	    // update the PTBDD register with the IO_DIR content
    PTBPE = registers[IO_PUE];	    // update the PTBPE register with the IO_PUE content
    registers[IO_IN] = PTBD;		    // update the IO_IN register with the PTBD content
    PTBD = registers[IO_OUT];	      // update the PTBD register with the IO_OUT content
  }
}

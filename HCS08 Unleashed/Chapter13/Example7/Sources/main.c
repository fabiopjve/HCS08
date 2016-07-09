/* 

   DEMO9S08QG8 - I2C master reader for the I2C ADC and I/O expander
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

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h"		/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define ADC_ID      0x90
#define ADC_ADR     0

#define ADCH        0
#define ADCR_HIGH   1
#define ADCR_LOW    2
#define IO_DIR      3
#define IO_OUT      4
#define IO_IN       5
#define IO_PUE      6

#define RD          1
#define WR          0

#define LED         PTBD_PTBD7


enum ei2c_states 
{
  I2C_IDLE,
  I2C_START, I2C_REPEATED_START,
  I2C_WRITE_REGISTER,
  I2C_WRITE_BYTE,
  I2C_DUMMY_READ, I2C_READ_BYTE,
  I2C_STOP,
  I2C_SLAVE_REG_WR, I2C_SLAVE_REG_RD  
};

char i2c_buffer[3], command_ready;
unsigned int adc_result;

enum ei2c_states i2c_fsm(char new_state);

// MCU initialization
void MCU_init(void)
{
  SOPT1 = bBKGDPE;        	// Enable debug pin
  ICSSC = NV_FTRIM;       	// configure FTRIM value
  ICSTRM = NV_ICSTRM;     	// configure TRIM value
  // BUSCLK is now 4 MHz
  // Now we configure the I2C interface:
  // I2C clock is 4MHz*1/40 = 100kHz, SDA hold = 9/4MHz=1.125us
  IICF = I2C_M1 | 0x0B;		// set the I2C bus speed (100kHz)
  IICC = bIICEN | bIICIE; 	// enable the I2C interface
  // Configure the I/O pins
  PTBDD = BIT_7;          	// PTB7 as an output
  EnableInterrupts;
}

// This is the I2C interrupt servicing function
void interrupt VectorNumber_Viic iic_isr(void)
{
  // verify the source of I2C interrupt:
  if (IICS_TCF)           	// a transfer was completed
  {
    i2c_fsm(0);           	// call I2C state machine
  }
  if (IICS_ARBL)          	// the master lost the arbitration
  {
    IICS_ARBL = 1;        	// clear ARBL
    i2c_fsm(I2C_IDLE);    	// put the I2C state machine in idle
  }
  IICS_IICIF = 1;         	// clear the interrupt flag
}

//*************************************************************************
// I2C finite state machine
// Usage:
//  i2c_fsm(0) to read the current state
//  i2c_fsm(x) to change the current state
// I2C buffer structure:
// i2c_buffer[0] : slave address
// i2c_buffer[1] : slave register
// i2c_buffer[2] : data to be written to the slave register (or read from it)
enum ei2c_states i2c_fsm(char new_state)
{
  static enum ei2c_states i2c_state;
  static char current_command;
  if (new_state == I2C_SLAVE_REG_RD)
  {
    current_command = RD;           	// current command is read
    i2c_state = I2C_START;          	// go to I2C_START state
  }
  if (new_state == I2C_SLAVE_REG_WR)
  {
    current_command = WR;           	// current command is write
    i2c_state = I2C_START;          	// go to I2C_START state
  }
  // if the TCF flag is not set, do not process the state machine
  // and return now with the current state
  if (!IICS_TCF) return (i2c_state);
  IICS_TCF = 1;                     	// clear TCF flag

  switch (i2c_state)
  {
    //*******************************************************************    
    // I2C in idle state
    case I2C_IDLE:
      break;
    //*******************************************************************      
    // send a start condition
    case I2C_START:
      IICC = bIICEN | bIICIE | bMST | bTX;
      IICD = i2c_buffer[0];         	 // send the address field
      i2c_state = I2C_WRITE_REGISTER; // next state
      break;
    //*******************************************************************      
    // send the register number
    case I2C_WRITE_REGISTER:
      IICD = i2c_buffer[1];         	// send the register number
      // now, depending on the current command being executed, the state
      // machine can take one of two paths
      if (current_command==WR)
        i2c_state = I2C_WRITE_BYTE; 	// if it is a write command
      else
        i2c_state = I2C_REPEATED_START; // if it is a read command
      break;
    //*******************************************************************      
    // write one byte of data into the slave device
    case I2C_WRITE_BYTE:
      IICD = i2c_buffer[2];         	// write data into the slave
      i2c_state = I2C_STOP;         	// next state
      break;
    //*******************************************************************      
    // send a repeated start (to change data direction on the slave)
    case I2C_REPEATED_START:
      IICC_RSTA = 1;                	// repeated start
      IICD = i2c_buffer[0] | RD;    	// send the address field (R/W = RD)
      i2c_state = I2C_DUMMY_READ;   	// next state
      break;
    //*******************************************************************
    // dummy read (start the memory read operation)
    case I2C_DUMMY_READ:
      IICC_TX = 0;                  	// set I2C to receive mode
      i2c_buffer[2] = IICD;         	// dummy read
      i2c_state = I2C_READ_BYTE;    	// next state
      break;
    //*******************************************************************
    // read one byte of data from the EEPROM      
    case I2C_READ_BYTE:
      IICC_TXAK = 1;                	// send NACK on the next read
      i2c_buffer[2] = IICD;         	// read data from the EEPROM
      i2c_state = I2C_STOP;         	// go to I2C_STOP state
      break;
    //*******************************************************************      
    case I2C_STOP:
      IICC_MST = 0;                 	// send a stop (go to slave mode)
      i2c_state = I2C_IDLE;         	// next state         
      command_ready = 1;            	// set the command_read flag
      break;
  }
  return (i2c_state);
}

// Read a slave register
char slave_register_read(char reg)
{
  char temp;
  while (i2c_fsm(0)!=I2C_IDLE);     	// wait for the I2C interface to be idle
  // write the slave device ID and address into the buffer
  i2c_buffer[0] = ADC_ID | ADC_ADR;
  i2c_buffer[1] = reg;			// write the register number into the buffer
  command_ready = 0;                	// clear the command_ready flag
  i2c_fsm(I2C_SLAVE_REG_RD);        	// set the I2C state machine
  for (temp=250; temp; temp--);
  while (!command_ready);           	// wait until the command is completed
  return (i2c_buffer[2]);           	// return the data read
}


// Write into a slave register
void slave_register_write(char reg, char data)
{
  char temp;
  while (i2c_fsm(0)!=I2C_IDLE);     	// wait for the I2C interface to be idle
  // write the slave device ID and address into the buffer
  i2c_buffer[0] = ADC_ID | ADC_ADR;
  i2c_buffer[1] = reg;			// write the register number into the buffer
  i2c_buffer[2] = data;			// write the data into the buffer
  i2c_fsm(I2C_SLAVE_REG_WR);		// set the I2C state machine
  for (temp=250; temp; temp--);
}

void main(void)
{
  unsigned int temp;
  MCU_init();             			// initialize the MCU
  // set PTB6 and PTB7 as outputs on the slave device
  slave_register_write(IO_DIR,0xC0);
  slave_register_write(IO_OUT,0xFF);  	// clear outputs on the slave device
  slave_register_write(ADCH,0);         // select slave analog channel 0
  while(1)
  {
    slave_register_write(IO_OUT,BIT_7);
    adc_result = slave_register_read(ADCR_HIGH) << 8;
    adc_result |= slave_register_read(ADCR_LOW);
    if (adc_result<511) LED = 1; else LED = 0;
    for (temp = 15000; temp; temp--);
    slave_register_write(IO_OUT,BIT_6);
    for (temp = 15000; temp; temp--);
  }
}

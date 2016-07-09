// DEMO9S08QG8 - I2C example 2 (I2C EEPROM interface using interrupts and FSM)
// EEPROM SDA pin connected to PTA2/SDA
// EEPROM SCL pin connected to PTA3/SCL
// By Fábio Pereira - fabio@sctec.com.br (01/10/08)
// Joinville - SC - Brasil

#include <hidef.h> 		    /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h"  		  // This is our definition file!

#define EEPROM_ID   0xA0
#define EEPROM_ADR  0
#define RD          1
#define WR          0

enum ei2c_states 
{
  I2C_IDLE,
  I2C_START, I2C_REPEATED_START,
  I2C_WRITE_HIGHER_ADDRESS, I2C_WRITE_LOWER_ADDRESS,
  I2C_WRITE_BYTE,
  I2C_DUMMY_READ, I2C_READ_BYTE,
  I2C_STOP,
  I2C_EEPROM_WR, I2C_EEPROM_RD  
};

unsigned int eeprom_address;
char eeprom_data, operation;
char i2c_buffer[4], command_ready;

enum ei2c_states i2c_fsm(char new_state);

// MCU initialization
void mcu_init(void)
{
  SOPT1 = bBKGDPE;        // Enable debug pin
  ICSSC = NV_FTRIM;       // configure FTRIM value
  ICSTRM = NV_ICSTRM;     // configure TRIM value
  // BUSCLK is now 4 MHz
  // Now we configure the I2C interface:
  // I2C clock is 4MHz*1/40 = 100kHz, SDA hold = 9/4MHz=1.125us
  IICF = I2C_M1 | 0x0B;
  IICC = bIICEN | bIICIE; // enable the I2C interface
  EnableInterrupts;
}

// This is the I2C interrupt servicing function
void interrupt VectorNumber_Viic iic_isr(void)
{
  // verify the source of I2C interrupt:
  if (IICS_TCF)           // a transfer was completed
  {
    i2c_fsm(0);           // call I2C state machine
  }


  if (IICS_ARBL)          // the master lost the arbitration
  {
    IICS_ARBL = 1;        // clear ARBL
    i2c_fsm(I2C_IDLE);    // set the I2C state machine to idle state
  }
  IICS_IICIF = 1;         // clear the interrupt flag
}

//*************************************************************************
// I2C finite state machine
// Usage:
//    i2c_fsm(0) to read the current state
//    i2c_fsm(x) to change the current state
// I2C buffer structure:
// i2c_buffer[0] : slave address
// i2c_buffer[1] : EEPROM higher address byte
// i2c_buffer[2] : EEPROM lower address byte
// i2c_buffer[3] : data to be written to the EEPROM (or read from it)
enum ei2c_states i2c_fsm(char new_state)
{
  static enum ei2c_states i2c_state;
  static char current_command;
  if (new_state == I2C_EEPROM_RD)
  {
    current_command = RD;           // current command is read
    i2c_state = I2C_START;          // go to I2C_START state
  }
  if (new_state == I2C_EEPROM_WR)
  {
    current_command = WR;           // current command is write
    i2c_state = I2C_START;          // go to I2C_START state
  }

  // if the TCF flag is not set, do not process the state machine
  // and return now with the current state
  if (!IICS_TCF) return (i2c_state);
  IICS_TCF = 1;                     // clear TCF flag
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
      IICD = i2c_buffer[0];         // send the address field
      i2c_state = I2C_WRITE_HIGHER_ADDRESS; // next state
      break;
    //*******************************************************************      
    // send the higher address byte
    case I2C_WRITE_HIGHER_ADDRESS: 
      IICD = i2c_buffer[1];         // send the higher address byte
      i2c_state = I2C_WRITE_LOWER_ADDRESS;  // next state 
      break;
    //*******************************************************************      
    // send the lower address byte
    case I2C_WRITE_LOWER_ADDRESS:
      IICD = i2c_buffer[2];         // send the lower address byte
      // now, depending on the current command being executed, the state
      // machine can take one of two possible paths
      if (current_command==WR)
        i2c_state = I2C_WRITE_BYTE; // if it is a write command
      else
        i2c_state = I2C_REPEATED_START; // if it is a read command
      break;
    //*******************************************************************      
    // write one byte of data in the EEPROM
    case I2C_WRITE_BYTE:
      IICD = i2c_buffer[3];         // write data to the EEPROM
      i2c_state = I2C_STOP;         // next state
      break;


    //*******************************************************************      
    // send a repeated start (to change data direction on the EEPROM)
    case I2C_REPEATED_START:
      IICC_RSTA = 1;                // repeated start
      IICD = i2c_buffer[0] | RD;    // send the address field (R/W = RD)
      i2c_state = I2C_DUMMY_READ;   // next state
      break;
    //*******************************************************************
    // dummy read (start the memory read operation)
    case I2C_DUMMY_READ:
      IICC_TX = 0;                  // set I2C to receive mode
      i2c_buffer[3] = IICD;         // dummy read
      i2c_state = I2C_READ_BYTE;    // next state
      break;
    //*******************************************************************
    // read one byte of data from the EEPROM      
    case I2C_READ_BYTE:
      IICC_TXAK = 1;                // send NACK on the next read
      i2c_buffer[3] = IICD;         // read data from the EEPROM
      i2c_state = I2C_STOP;         // go to I2C_STOP state
      break;
    //*******************************************************************
    // send a stop and go to slave mode      
    case I2C_STOP:
      IICC_MST = 0;                 // send a stop (go to slave mode)
      i2c_state = I2C_IDLE;         // next state         
      command_ready = 1;            // set the command_read flag
      break;
  }
  return (i2c_state);
}
// Read a byte from the EEPROM
char eeprom_byteread(unsigned int address)
{
  while (i2c_fsm(0)!=I2C_IDLE);     // wait for the I2C interface to be idle
  i2c_buffer[0] = EEPROM_ID | EEPROM_ADR;
  i2c_buffer[1] = address >> 8;
  i2c_buffer[2] = address;
  command_ready = 0;                // clear the command_ready flag
  i2c_fsm(I2C_EEPROM_RD);           // set the I2C state machine
  while (!command_ready);           // wait until the command is completed
  return (i2c_buffer[3]);           // return the data read
}
// Write a byte into the EEPROM
void eeprom_bytewrite(unsigned int address, char data)
{
  while (i2c_fsm(0)!=I2C_IDLE);     // wait for the I2C interface to be idle
  i2c_buffer[0] = EEPROM_ID | EEPROM_ADR;
  i2c_buffer[1] = address >> 8;
  i2c_buffer[2] = address;
  i2c_buffer[3] = data;
  i2c_fsm(I2C_EEPROM_WR);           // set the I2C state machine
}
void main(void)
{
  mcu_init();           	// initialize the MCU
  eeprom_address = 0;
  eeprom_data = 0;
  operation = 0;
  while(1)
  {
    if (operation==1)     	// write operation
    {
      eeprom_bytewrite(eeprom_address, eeprom_data);
      operation = 0;
    }
    if (operation==2)     	// read operation
    {
      eeprom_data = eeprom_byteread(eeprom_address);
      operation = 0;
    }
  }
}

/*
  Serial servo controller with the MC9S08QG8
  By Fábio Pereira (12/04/07)
  Joinville - SC - Brasil
  www.sctec.com.br
  
  PTB0 - serial rx data
  PTB1 - servo 1 output
  PTB2 - servo 2 output
  PTB3 - servo 3 output
  PTB4 - servo 4 output
  PTB5 - servo 5 output
  PTB6 - servo 6 output
  PTB7 - servo 7 output
  PTA0 - servo 8 output
  PTA1 - servo 9 output
  PTA2 - servo 10 output
  
  ICS operating at 16 MHz (8 MHz BUSCLK)
  TPM operating from BUSCLK divided by 8 (1us counts)
  SCI operating at 9600 bps (RX only)
*/

#include <hidef.h>	    /* for EnableInterrupts macro */
#include "derivative.h"	/* include peripheral declarations */
#include "hcs08.h"    	// this is our definition file	

#define SERVO1  PTBD_PTBD1
#define SERVO2  PTBD_PTBD2
#define SERVO3  PTBD_PTBD3
#define SERVO4  PTBD_PTBD4
#define SERVO5  PTBD_PTBD5
#define SERVO6  PTBD_PTBD6
#define SERVO7  PTBD_PTBD7
#define SERVO8  PTAD_PTAD0
#define SERVO9  PTAD_PTAD1
#define SERVO10 PTAD_PTAD2

#define MINIMUM_CYCLE_uS  1000	/* this is the minimum pulse width in us     */
#define MAXIMUM_CYCLE_uS  2000	/* this is the maximum pulse width in us     */
#define INTERVAL_uS       30000	/* this is the interval between pulses in us */
#define MAX_SERVO_POS     254	  /* this is the maximum servo position        */

#define MULT_VALUE        (((MAXIMUM_CYCLE_uS-MINIMUM_CYCLE_uS)*10)/MAX_SERVO_POS)

char servo_pos[10];         	// array for each servo position
unsigned int new_cycle[10]; 	// array for each servo cycle (us)
char servo_index;	// current active servo

#define RX_BUF_SIZE   16	// total rx buffer size in bytes

volatile char rx_buffer[RX_BUF_SIZE];
volatile char rx_buf_read_pointer, rx_buf_write_pointer;
volatile char rx_com_state;

#pragma DATA_SEG __DIRECT_SEG MY_ZEROPAGE
struct
{
  char rx_flag      : 1;  	// new data received
  char seq_complete : 1;  	// servo sequence is completed
} __near flags;
#pragma DATA_SEG DEFAULT

/**************************************************************************
  Function prototypes
**************************************************************************/
void com_state_machine(void);
void init(void);

// TPM channel 0 ISR - clears all outputs after a specified delay
void interrupt VectorNumber_Vtpmch0 ISR_channel0(void)
{
  TPMC0SC_CH0F = 0;   	// clear interrupt (CH0F)
  // Now we clear all output pins (all servo signals go low)
  PTBD = 0;
  PTAD = 0;
}

/**************************************************************************
	TPM channel 1 ISR:
	- Controls the servo sequencing
	- Activates each servo output in the sequence
	- Updates the channel 0 compare register (current servo pulse width)
	- Updates the channel 1 compare register (next servo)
	- Flags the start of global servo width calculation
	- Controls the delay between pulse sequences
**************************************************************************/
void interrupt VectorNumber_Vtpmch1 ISR_channel1(void)
{
  TPMC1SC_CH1F = 0;   	// clear interrupt (CH1F)
  // the following switch cycles through all servos,
  // enabling the corresponding output pin
  switch (servo_index)
  {
    case 0: 		// Servo 1 (output high)
      SERVO1 = 1; break;
    case 1: 		// Servo 2 (output high)
      SERVO2 = 1; break;
    case 2: 		// Servo 3 (output high)
      SERVO3 = 1; break;
    case 3: 		// Servo 4 (output high)
      SERVO4 = 1; break;
    case 4: 		// Servo 5 (output high)
      SERVO5 = 1; break;
    case 5: 		// Servo 6 (output high)
      SERVO6 = 1; break;
    case 6: 		// Servo 7 (output high)
      SERVO7 = 1; break;                                    
    case 7: 		// Servo 8 (output high)
      SERVO8 = 1; break;
    case 8: 		// Servo 9 (output high)
      SERVO9 = 1; break;
    case 9: 		// Servo 10 (output high)
      SERVO10 = 1; break;                 
  }
  if (servo_index<10) 	// if the index refers to a valid servo
  {
    // then we update channel 0 compare register to the servo cycle value
    TPMC0V = TPMCNT + new_cycle[servo_index] - 19;
    // we also update channel 1 compare register to generate an interrupt
    // to initiate the next servo update
    TPMC1V = TPMCNT + MAXIMUM_CYCLE_uS + 10;
    servo_index++;      	// next servo to be updated
  } 
  else
  {
    // if the servo index does not point to a valid servo, then we
    // finished the current sequence
    flags.seq_complete = 1;
    // we update the channel 1 compare register to generate a delay
    // corresponding to the remaining interval time
    TPMC1V = TPMCNT + INTERVAL_uS - (MAXIMUM_CYCLE_uS * 10);
    servo_index = 0;  	// restart the servo sequence
  }
}

/**************************************************************************
  SCI reception interrupt servicing function
  Received data is stored into the receive buffer (rx_buffer)
**************************************************************************/
void interrupt VectorNumber_Vscirx ISR_scirx(void)
{
  char temp;
  temp = SCIS1; 		// read sci status for the received char
  if (temp & 7) 		// is there any error ?
  {
    // yes, read the char and return (no data is stored into the buffer)
    temp = SCID;  		// dummy read of rx data for clearing interrupt flag
    return;
  }
  // an error free char was received, write it into the buffer
  rx_buffer[rx_buf_write_pointer++] = SCID;

  // verify if the buffer pointer exceeded the buffer size, if so, restart
  // pointer to zero (this is a circular buffer)
  if (rx_buf_write_pointer>=RX_BUF_SIZE) rx_buf_write_pointer = 0;
  flags.rx_flag = 1;  	// set rx flag
}


/************************************************************************** 
  State machine for controlling the communication with the host computer
  One state for each frame part:
  0 - header (0xFF)	
  1 - servo number	
  2 - cycle value
**************************************************************************/
void com_state_machine(void)
{
  unsigned char data;
  static unsigned char servo;
  data = rx_buffer[rx_buf_read_pointer++];
  if (rx_buf_read_pointer>=RX_BUF_SIZE) rx_buf_read_pointer = 0;
  if (rx_buf_read_pointer==rx_buf_write_pointer) flags.rx_flag = 0;
  switch (rx_com_state)
  {
    case 0: 	// wait for the header
      if (data==0xFF) rx_com_state++;	// if the header is ok, advance to the next state
      break;
    case 1: 	// wait for the servo number
      if (data<10)  	// if it is a valid servo 
      {
        servo = data;
        rx_com_state++;	// advance to the next state
      } else rx_com_state = 0;
      break;
    case 2: 	// wait for the cycle value
      servo_pos[servo] = data;	// store it into the array
      rx_com_state=0; 	// restart rx state machine
      break;
  }
}


// MCU initialization function
void MCU_init(void)
{
	SOPT1 = bBKGDPE;	      // enable debug pin
 	SOPT2 = 0;             	// initialize SOPT2
	// Trim the internal oscillator
  ICSTRM = NV_ICSTRM;
 	ICSSC = NV_FTRIM;
	ICSC2 = 0;
 	// CPU = 16MHz, BUSCLK = ICSCLK/2 = 8MHz
	PTAD = 0;
	PTADD = 0xFF;
	PTAPE = BIT_5;	        // PTA5 internal pull up enabled
	PTBD = 0;
	PTBDD = 0xFE;
	// TPM config
	TPMSC = TPM_BUSCLK + TPM_DIV8; // prescaler /8 (1us @ 8MHz)
	// Channel 0 config (interrupt on compare)	
	TPMC0SC = bCHIE | TPM_COMPARE_INT;
	// Channel 1 config (interrupt on compare)
	TPMC1SC = bCHIE | TPM_COMPARE_INT;
	// Initialize channel 1 compare register
	TPMC1V = TPMCNT + 10;  
	// SCI config (9600 bps, RX only)
	SCIBD = 52; 	          // 9600 bps
	SCIC2 = bRIE | bRE;	    // enable RX module and RX interrupt
	// Variables initialization
	flags.rx_flag = 0;
	flags.seq_complete = 0;
	servo_index = 10;
	EnableInterrupts;	      // enable interrupts (CCR:I = 0)
}




void main(void) 
{
  char index;
  MCU_init(); 	// initialize the MCU
  while(1)
  {
		// if there is new data on rx buffer, call the state machine
		if (flags.rx_flag) com_state_machine();
		// if the servo sequence is completed, calculate new cycles
		if (flags.seq_complete)
		{
		  for (index=0; index<10; index++)
		    new_cycle[index]=MINIMUM_CYCLE_uS + (servo_pos[index]*MULT_VALUE)/10;
		  flags.seq_complete = 0;	// clear seq_complete flag
		}
  }
}

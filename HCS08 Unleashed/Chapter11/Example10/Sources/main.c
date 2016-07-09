// DEMO9S08QE128 - SCI example (circular buffer)
// RxD connected to PTB0
// TxD connected to PTB1
// COM_EN connected to PTC5

#include <hidef.h>        	/* for EnableInterrupts macro */
#include "derivative.h"   	/* include peripheral declarations */
#include "hcs08.h"        	// This is our header file!

#define LED2    PTCD_PTCD0
#define COM_EN  PTCD_PTCD5

#define TX_BUF_SIZE   32  	// TX buffer size = 32 characters
#define RX_BUF_SIZE   32  	// RX buffer size = 32 characters

volatile char txbuffer[TX_BUF_SIZE], rxbuffer[RX_BUF_SIZE];
volatile char tx_buf_read_pointer, tx_buf_write_pointer;
volatile char rx_buf_read_pointer, rx_buf_write_pointer;


#pragma DATA_SEG __DIRECT_SEG MY_ZEROPAGE
struct
{
  char rx_flag      : 1;  	// there are characters in the rx buffer
  char tx_buf_empty : 1;  	// the tx buffer is empty
} __near flags;
#pragma DATA_SEG DEFAULT

/*
  SCI receive interrupt servicing function
  The received characters are stored into the receive buffer  
*/
void interrupt VectorNumber_Vsci1rx isr_sci1rx(void)
{
  char temp;
  temp = SCI1S1;		// read SCI1S1 to check for receive errors
  if (temp & 7)   		// is there any rx errors?
  { // yes, make a dummy read of the rx data and return
    temp = SCI1D;  		// dummy read of rx data to clear the interrupt flag
    return;
  }
  // read and store the received character, increment the rx write pointer
  rxbuffer[rx_buf_write_pointer++] = SCI1D;
  if (rx_buf_write_pointer>=RX_BUF_SIZE) rx_buf_write_pointer = 0;
  flags.rx_flag = 1;  		// set the rx_flag (there is data into the rx buffer)
}

/*
  SCI transmit interrupt servicing function
  Get characters from the transmit buffer and transmit them through the SCI
  If there are no more characters to be transmitted, disable the TX interrupt
*/  
void interrupt VectorNumber_Vsci1tx isr_scitx(void)
{
  if (tx_buf_read_pointer==tx_buf_write_pointer)
  {
    // if read and write pointers are equal, there is no more data to transmit
    // disable tx interrupt and set the tx buffer empty flag
    SCI1C2_TIE = 0;		// disable the TX interrupt
    flags.tx_buf_empty = 1;	// tx buffer is empty
  } else
  {
    if (SCI1S1_TDRE)
    {
      // if there is any data in the tx buffer, transmit the next char and
      // advance the tx buffer read pointer
      SCI1D = txbuffer[tx_buf_read_pointer++];
      // if the tx read buffer is bigger than the buffer size, set the pointer to zero
      if (tx_buf_read_pointer>=TX_BUF_SIZE) tx_buf_read_pointer = 0;
    }
  }
}

// Function for writing a string into the sci tx buffer
void write_string_sci_buffer(unsigned char *data)
{
  flags.tx_buf_empty = 0;
  while (*data)
  {
    txbuffer[tx_buf_write_pointer++] = *data++;
    // if the tx write buffer is bigger than the buffer size, set the pointer to zero
    if (tx_buf_write_pointer>=TX_BUF_SIZE) tx_buf_write_pointer = 0;
    SCI1C2_TIE = 1;		// enable TX interrupt
  }  
}

// Function for writing a char into the sci tx buffer
void write_char_sci_buffer(unsigned char data)
{
  flags.tx_buf_empty = 0;   	// the tx buffer is not empty anymore
  // write the data into the tx buffer and increment the tx write buffer
  txbuffer[tx_buf_write_pointer++] = data;
  // if the tx write buffer is bigger than the buffer size, set the pointer to zero
  if (tx_buf_write_pointer>=TX_BUF_SIZE) tx_buf_write_pointer = 0;
  SCI1C2_TIE = 1;		// enable TX interrupt
}


// Function for reading a character from the rx buffer
char read_char_sci_buffer(void)
{
  char temp;
  temp = rxbuffer[rx_buf_read_pointer++];
  // if the read pointer is bigger than the buffer size, set the pointer to zero
  if (rx_buf_read_pointer>=RX_BUF_SIZE) rx_buf_read_pointer = 0;
  // if the read pointer reached the write pointer, clear the rx_flag
  SCI1C2_RIE = 0;     // disable the receive interrupt
  if (rx_buf_read_pointer==rx_buf_write_pointer) flags.rx_flag = 0;
  SCI1C2_RIE = 1;     // re-enable the receive interrupt
  return (temp);    	// return the character
}

void main(void)
{
  char rxchar;
  SOPT1 = bBKGDPE;        	// enable the debug pin
  PTCDD = BIT_0 | BIT_5;    // PTC0 and PTC5 as outputs
  // Following a reset, BUSCLK = 4MHz 
  SCI1BD = 26;             	// SCI baud rate = 4MHz/(16*26)=9615 bps
  // enable TX and RX section, RX and TX interrupts
  SCI1C2 = bTIE | bRIE | bTE | bRE;
  COM_EN = 1;               // enable MAX3218
  write_string_sci_buffer("Hello World!");  // write into the tx buffer
  EnableInterrupts;
  while(1)
  {
    while (!flags.rx_flag);	// wait for a new character to be received
    rxchar = read_char_sci_buffer();
    write_char_sci_buffer(rxchar);
    LED2 = !LED2;         	// toggle the led
    if (rxchar==13) write_string_sci_buffer("HCS08 rules!\r\n"); 
  }
}

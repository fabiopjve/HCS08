// DEMO9S08QG8 - SCI TX and RX example 2

#include <hidef.h>        /* for EnableInterrupts macro */
#include "derivative.h"   /* include peripheral declarations */
#include "hcs08.h"        // This is our header file!

#define LED2  PTBD_PTBD7

// This is a simple string to serial function
void SCI_send_string(char *string)
{
  while (*string)         // while the current char of the string is not null
  {
    while (!SCIS1_TDRE);  // wait for the transmit buffer to be empty
    SCID = *string;       // write the current char into the transmit buffer
    string++;             // increment the current char position within the string
  }
}

void main(void)
{
  char rxchar;
  SOPT1 = bBKGDPE;        // enable the debug pin
  PTBDD_PTBDD7 = 1;       // PTB7 as an output
  // Following a reset, BUSCLK = 4MHz 
  SCIBD = 26;             // SCI baud rate = 4MHz/(16*26)=9615 bps
  SCIC2 = bTE | bRE;      // enable the transmitter section
  SCI_send_string("Hello World!");  // write into the serial port ...
  while(1)
  {
    while (!SCIS1_RDRF);  // wait for a new character to be received
    while (!SCIS1_TDRE);  // wait for the transmit buffer to be empty
    rxchar = SCID;        // read the received char
    SCID = rxchar;        // send the char back to the host PC
    LED2 = !LED2;         // toggle the led
    if (rxchar==13) SCI_send_string("HCS08 rules!\r\n"); 
  }
}

// DEMO9S08QG8 - Bit-banged UART
// TxD pin is PTB1
// RxD pin is PTB0
// BUSCLK = 4MHz
// Baud-rate = 9600 bps
// By Fábio Pereira - fabio@sctec.com.br
// www.sctec.com.br

#include <hidef.h> 		    	/* for EnableInterrupts macro */
#include "derivative.h" 		/* include peripheral declarations */
#include "hcs08.h"  		  	// This is our definition file!

#define TX_PIN    PTBD_PTBD1		// PTB1 as the TX pin
#define RX_PIN    PTBD_PTBD0		// PTB0 as the RX pin
#define RX_KBI    BIT_4		// PTB0(RX) is the 4th KBI input

#define BAUDRATE  9600            	// this is the desired baud-rate
#define TPMCK     1000000         	// the TPMCK clock
#define BITTIME   TPMCK/BAUDRATE  	// bit time calculation

char tx_data, rx_data;

#pragma DATA_SEG __DIRECT_SEG MY_ZEROPAGE
struct
{
  char tx_enable    : 1;  		// transmitter is enabled
  char rx_flag      : 1;  		// character received
} __near flags;
#pragma DATA_SEG DEFAULT

// This is the timer isr that generates the transmittion timing
void interrupt VectorNumber_Vtpmch0 tpmch0_isr(void)
{
  static char tx_state;
  #pragma DATA_SEG __DIRECT_SEG MY_ZEROPAGE
  static __near char buffer;
  #pragma DATA_SEG DEFAULT
  TPMC0SC_CH0F = 0;     		// clear interrupt flag
  TPMC0V += BITTIME;    		// next compare in 104us
  if (flags.tx_enable)
  {
    if (!tx_state)  			// it is the start bit
    {
      TX_PIN = 0;
      buffer = tx_data;
      tx_state++;
    } 
    else
    if (tx_state<9) 			// it is a data bit
    {
      TX_PIN = buffer & 1;  		// output the LSB
      buffer >>= 1; 			// shift buffer one bit to the left
      tx_state++;
    } 
    else
    if (tx_state==9)			// its the stop bit
    {
      TX_PIN = 1;
      tx_state++; 
    } 
    else          			// transmission completed
    {
      tx_state = 0; 			// restart tx_state
      flags.tx_enable = 0;		// disable the transmitter
    }    
  }
}


// This is the timer isr that generates the receiver timing
void interrupt VectorNumber_Vtpmch1 tpmch1_isr(void)
{
  static char rx_state;
  #pragma DATA_SEG __DIRECT_SEG MY_ZEROPAGE
  static __near char buffer;
  #pragma DATA_SEG DEFAULT
  TPMC1SC_CH1F = 0; 			// clear interrupt flag
  TPMC1V += BITTIME;    		// next compare in 104us
  if (!rx_state)  			// it is the start bit ?
  {
    if (!RX_PIN)
    {
      buffer = 0;			// it is a start bit
      rx_state++;			// advance to the next state (bit 0)
    } 
    else
    {
      KBIPE = RX_KBI; 		// re-enable the RX pin interrupt
      KBISC_KBACK = 1;  		// clear interrupt flag    
      TPMC1SC = 0;      
    }
  }
  else if (rx_state<9) 		// its a data bit
  {
    buffer >>= 1; 			// shift the buffer one bit to the left
    if (RX_PIN) buffer |= 128;
    rx_state++;
  } 
  else
  {
    if (RX_PIN) 			// is it the stop bit ?
    {
      rx_data = buffer;
      flags.rx_flag = 1;		// set the receiver flag
    } 
    KBIPE = RX_KBI; 			// re-enable the RX pin interrupt
    KBISC_KBACK = 1;  		// clear interrupt flag    
    rx_state = 0;
    TPMC1SC = 0;
  }
}

// This is the timer isr that generates the receiver timing
void interrupt VectorNumber_Vkeyboard kbi_isr(void)
{
  KBISC_KBACK = 1;  			// clear interrupt flag
  TPMC1SC = bCHIE | TPM_COMPARE_INT;
  TPMC1V = TPMCNT + BITTIME/2-1;
  KBIPE_KBIPE4 = 0; 			// disable the RX pin interrupt
}

// This is a simple string to serial function
void send_serial_string(char *string)
{
  while (*string)         		// while the current char of the string is not null
  {
    while (flags.tx_enable);  	// wait for the transmitter to be idle
    tx_data = *string;       	// write the current char into the transmit buffer
    flags.tx_enable = 1;
    string++;             	// increment the current char position within the string
  }
}

void mcu_init(void)
{
  SOPT1 = bBKGDPE;        		// Enable debug pin
  ICSSC = NV_FTRIM;       		// configure FTRIM value
  ICSTRM = NV_ICSTRM;     		// configure TRIM value
  ICSC2 = 0;
  // BUSCLK is now 8 MHz
  // Configure the I/O pins
  PTBDD = BIT_1;          		// PTB1 as an output
  // Configure the TPM for the bit time generation:
  TPMSC = TPM_BUSCLK | TPM_DIV8;  	// TPMCK = 1MHz
  TPMC0SC = bCHIE | TPM_COMPARE_INT;

  // Configure the RX pin interrupt (KBI module)
  KBISC = bKBIE;
  KBIPE = RX_KBI;  			// bit 4 = PTB0
  EnableInterrupts;
  flags.tx_enable = 0;
  flags.rx_flag = 0;
  TX_PIN = 1; 				// TX pin is idle
}

void main(void)
{
  mcu_init();
  send_serial_string("This is a test!\r\n");
  while (1)
  {
    if (flags.rx_flag)
    {
      tx_data = rx_data;
      flags.rx_flag = 0;
      flags.tx_enable = 1;
    }
  }  
}

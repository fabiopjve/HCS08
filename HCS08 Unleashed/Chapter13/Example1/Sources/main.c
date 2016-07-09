/*
  LCD demonstration - MC9S08QG8 analog bar graph
  
  By Fábio Pereira (01/21/08)
  Joinville - SC - Brasil
  www.sctec.com.br

  LCD display connected to:
  DB0 - not connected
  DB1 - not connected
  DB2 - not connected
  DB3 - not connected
  DB4 - PTB4
  DB5 - PTB5
  DB6 - PTB6
  DB7 - PTB7
  E   - PTB1
  RS  - PTB0
  Trimpot connected to PTA0
*/  

#include <hidef.h>  	  	/* for EnableInterrupts macro */
#include "derivative.h"	/* include peripheral declarations */
#include "hcs08.h"	  	// this is our definition file

#define BUS_CLOCK 8000
#include "my_lcd.c"

unsigned int average;
unsigned int sum;
unsigned int timeout;

const char custom_char[] = 
{
  0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // one column
  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, // two columns
  0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, // three columns
  0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, // four columns
  0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F  // five columns  
};

// ADC ISR
void interrupt VectorNumber_Vadc adc_isr(void)
{
  sum = sum + ADCR - average;
  if (timeout) timeout--; // ADC embedded timer   
}

// LCD bar graph function
// "pix" = number of pixels to plot (0 up to 80)
void LCD_bargraph(char pix)
{
  char counter = 16;  // number of display columns
  while (pix)         // while pix>0
  {
    if (pix>=5)       // if there are more than 5 pixels
    {
      LCD_write_char(4);  // draw a black block
      counter--;      // one column less
      pix -= 5;       // five pixels less
    } else            // if there is less than 5 pixels
    {
      LCD_write_char(pix-1);  // draw a partial block
      pix = 0;        // no more pixels remaining
      counter--;      // one column less
    }    
  }
  // fill the remaining columns with white spaces
  while (counter--) LCD_write_char(' ');
}



void main(void)
{
  char aux;
  SOPT1 = bBKGDPE;	        // configure SOPT1 register, enable pin BKGD
  ICSSC = NV_FTRIM;         // configure FTRIM value
  ICSTRM = NV_ICSTRM;       // configure TRIM value
  ICSC2 = 0;                // ICSOUT = DCOOUT / 1
  // enable long sampling, 10-bit mode, ADICLK = 11b, ADCK = BUSCLK/16
  // ADC clock = 500 kHz - conversion time = 80us
  ADCCFG = bADLSMP | ADC_10BITS | ADC_BUSCLK_DIV2 | ADIV_8;
  // ADC in continuous mode, interrupt enabled, internal temperature sensor
  ADCSC1 = bAIEN | bADCO | ADC_CH0;   
  APCTL1 = bADPC0; // PTA0 as an analog input
  LCD_init(DISPLAY_8X5|_2_LINES,DISPLAY_ON|CURSOR_OFF|CURSOR_NOBLINK);
  // Now we set up the user custom characters
  LCD_send_byte(0,0x40);    // point AC to the CGRAM area
  for (aux=0; aux<sizeof(custom_char); aux++)
  {
    LCD_send_byte(1,custom_char[aux]);  // send the data to the LCD module
  }
  LCD_write_char('\f');     // clear the display
  LCD_pos_xy(0,0);          // set cursor to the first column of the first line
  LCD_write_string("HCS08 Unleashed!"); // write on the display
  LCD_pos_xy(0,1);          // set cursor to the second column of the second line
  sum = 0;
  average = 0;	
  EnableInterrupts;  
  while(1)
  {
    average = sum / 8;      // update the average
    if (!timeout)           // if the timeout expired
    {
      LCD_pos_xy(0,1);      // set cursor to the first column of the second line
      LCD_bargraph(average/13); // draw the bar graph
      timeout = 625;        // wait for 50ms (625 * 80us) 
    }
  }
}

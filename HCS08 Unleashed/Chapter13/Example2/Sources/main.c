/*
  LCD demonstration - MC9S08QG8 thermometer (internal temperature sensor)
  By Fábio Pereira (01/22/08)
  Joinville - SC - Brasil
  www.sctec.com.br

  LCD display module connected to:
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
*/  

#include <hidef.h>  	  	/* for EnableInterrupts macro */
#include "derivative.h"		/* include peripheral declarations */
#include "hcs08.h"	  	  // this is our definition file

#define BUS_CLOCK 8000
#include "my_lcd.c"

unsigned int average_temp, average_bandgap;
unsigned long int sum_temp, sum_bandgap;
unsigned int temperature;
unsigned int reference_temp;
unsigned int supply_voltage;
unsigned int timeout;
unsigned int m_slope;

void interrupt VectorNumber_Vtpmovf tpm_isr(void)
{
  TPMSC_TOF = 0;
  if (timeout) timeout--;
}

// ADC ISR
void interrupt VectorNumber_Vadc adc_isr(void)
{
  static char adc_seq;
  if (!adc_seq)
  {
    sum_temp = sum_temp + ADCR - average_temp;
    ADCSC1_ADCH = ADCH_BANDGAP;
    adc_seq++;
  } 
  else
  {
    sum_bandgap = sum_bandgap + ADCR - average_bandgap;
    ADCSC1_ADCH = ADCH_TEMP_SENSOR;
    adc_seq = 0;    
  }
    
}

// Converts a 16-bit value into a ASCII string
// "value" is the 16-bit value
// "*str" is a pointer to the string
// This is a very simple way to convert a number to decimal format
void bin_to_string(unsigned int value, char *str)
{
  char aux;
  unsigned int unit;
  aux = 0;
  str[0] = str[1] = str[2] = str[3] = str[4] = '0';
  str[5] = 0;
  unit = 10000;
  while (value)
  {
    if (value>=unit)
    {
      value -= unit;
      str[aux]++;
    } else
    {
      aux++;
      unit /= 10;
    }    
  }
  while (str[0]=='0')
  {
    for (aux=0; aux<=4; aux++) str[aux]=str[aux+1];
  }
}

void main(void)
{
  char string[10];
  SOPT1 = bBKGDPE;	        // configure SOPT1 register, enable pin BKGD
  ICSSC = NV_FTRIM;         // configure FTRIM value
  ICSTRM = NV_ICSTRM;       // configure TRIM value
  ICSC2 = 0;                // ICSOUT = DCOOUT / 1
  // BUSCLK = 16MHz/2 = 8MHz
  SPMSC1 = bBGBE;           // enable bandgap reference

  // enable long sampling, 10-bit mode, ADICLK = 11b, ADCK = BUSCLK/16
  // ADC clock = 500 kHz - conversion time = 80us
  ADCCFG = bADLSMP | ADC_10BITS | ADC_BUSCLK_DIV2 | ADIV_8;
  // ADC: interrupt enabled, bandgap reference
  ADCSC1 = bAIEN | ADCH_BANDGAP;
  // Configure TPM for 1ms periodic interrupt
  // TPMCK = 1MHz, modulo = 1000 (1MHz/1000 = 1000Hz = 1ms)
  TPMSC = bTOIE | TPM_BUSCLK | TPM_DIV8;
  TPMMOD = 999;
  LCD_init(DISPLAY_8X5|_2_LINES,DISPLAY_ON|CURSOR_OFF|CURSOR_NOBLINK);
  LCD_write_char('\f');     // clear the display
  LCD_pos_xy(0,0);          // set cursor to the first column of the first line
  LCD_write_string("HCS08 Unleashed!"); // write on the display
  LCD_pos_xy(1,1);          // set cursor to the second column of the second line
  LCD_write_string("Int temp = ");      // write on the display	
  timeout = 300;            // wait for 300ms
  sum_temp = 0;
  sum_bandgap = 0;
  average_temp = 0;	
  average_bandgap = 0;
  EnableInterrupts;  
  while(1)
  {
    average_temp = sum_temp / 64;       // update the average temperature
    average_bandgap = sum_bandgap / 64; // update the average bandgap value
    if (average_bandgap) supply_voltage = 12276/average_bandgap;
    if (supply_voltage) reference_temp = 7173/supply_voltage;
    if (!timeout)           // if the timeout expired
    {
      // read the averaged temperature, if it is higher than or equal to
      // the reference temperature (700mv approximately), then we use the 
      // cold slope formula
      if (average_temp>=reference_temp)     // cold slope
      {
        if (supply_voltage)
        {
          m_slope = 1684/supply_voltage;
          temperature = 25 - ((average_temp - reference_temp)*100)/m_slope;
        }
      } 
      else  
      { // if average is less than the reference temperature, then we 
        // use the hot slope formula
        if (supply_voltage)
        {
          m_slope = 1809/supply_voltage;
          temperature = 25 + ((reference_temp - average_temp)*100)/m_slope;
        }
      }
      bin_to_string(temperature,string);  // convert the temperature to string
      LCD_pos_xy(12,1);         // set cursor position
      LCD_write_string(string); // write the converted temperature on the LCD
      LCD_write_string("   ");  // clear the next characters after the temperature
      timeout = 300;            // wait for 300ms 
    }
  }
}

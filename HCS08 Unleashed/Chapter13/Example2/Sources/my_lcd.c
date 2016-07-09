//**************************************************************************
//* Basic Character LCD functions
//* By Fábio Pereira
//* 01/15/08
//**************************************************************************

// The following defines set the default pins for the LCD display
#ifndef LCD_ENABLE			 // if lcd_enable is not defined
#define LCD_ENABLE      PTBD_PTBD1	 // LCD enable pin on PTB1
#define LCD_ENABLE_DIR  PTBDD_PTBDD1	 // LCD enable pin direction
#define LCD_RS		    PTBD_PTBD0	 // LCD r/s pin on PTB0
#define LCD_RS_DIR	    PTBDD_PTBDD0	 // LCD r/s pin direction
#define LCD_D4          PTBD_PTBD4    // LCD data D4 pin
#define LCD_D4_DIR      PTBDD_PTBDD4  // LCD data D4 pin direction
#define LCD_D5          PTBD_PTBD5    // LCD data D5 pin
#define LCD_D5_DIR      PTBDD_PTBDD5  // LCD data D5 pin direction
#define LCD_D6          PTBD_PTBD6    // LCD data D6 pin
#define LCD_D6_DIR      PTBDD_PTBDD6  // LCD data D6 pin direction
#define LCD_D7          PTBD_PTBD7    // LCD data D7 pin
#define LCD_D7_DIR      PTBDD_PTBDD7  // LCD data D7 pin direction 
#endif

#define LCD_SEC_LINE    0x40    // Address of the second line of the LCD

// LCD configuration constants
#define CURSOR_ON       2
#define CURSOR_OFF      0
#define CURSOR_BLINK    1
#define CURSOR_NOBLINK  0
#define DISPLAY_ON      4
#define DISPLAY_OFF     0
#define DISPLAY_8X5     0
#define DISPLAY_10X5    4
#define _2_LINES        8
#define _1_LINE         0

// Display configuration global variable
static char lcd_mode;	

union ubyte
{
	char _byte;
	struct
	{
		char b0 : 1;
		char b1 : 1;
		char b2 : 1;
		char b3 : 1;
		char b4 : 1;
		char b5 : 1;
		char b6 : 1;
		char b7 : 1;
	} bit;
};

//**************************************************************************
//* Prototypes
//**************************************************************************
void LCD_delay_ms (unsigned char time);
void LCD_send_nibble(char data);
void LCD_send_byte(char address, char data);
void LCD_init(char mode1, char mode2);
void LCD_pos_xy(char x, char y);
void LCD_write_char(char c);
void LCD_write_string (char *c);
void LCD_display_on(void);
void LCD_display_off(void);
void LCD_cursor_on(void);
void LCD_cursor_off(void);
void LCD_cursor_blink_on(void);
void LCD_cursor_blink_off(void);

//**************************************************************************
//* A simple delay function (used by LCD functions)
//**************************************************************************
//* Calling arguments
//* unsigned char time: aproximate delay time in microseconds
//**************************************************************************
void LCD_delay_ms (unsigned char time)
{
	unsigned int temp;
	for(;time;time--) for(temp=(BUS_CLOCK/23);temp;temp--);
}

//**************************************************************************
//* Send a nibble to the LCD
//**************************************************************************
//* Calling arguments
//* char data : data to be sent to the display
//**************************************************************************
void LCD_send_nibble(char data)
{
	union ubyte my_union;
	my_union._byte = data;
	// Output the four data bits
	LCD_D4 = my_union.bit.b0;
	LCD_D5 = my_union.bit.b1;
	LCD_D6 = my_union.bit.b2;
	LCD_D7 = my_union.bit.b3;
	// pulse the LCD enable line
	LCD_ENABLE = 1;
	for (data=20; data; data--);
	LCD_ENABLE = 0;
}

//**************************************************************************
//* Write a byte into the LCD
//**************************************************************************
//* Calling arguments:
//* char address : 0 for instructions, 1 for data
//* char data : command or data to be written
//**************************************************************************
void LCD_send_byte(char address, char data)
{
  unsigned int temp;
	LCD_RS = address;               // config the R/S line
	LCD_ENABLE = 0;                 // set LCD enable line to 0
	LCD_send_nibble(data >> 4);     // send the higher nibble
	LCD_send_nibble(data & 0x0f);   // send the lower nibble
	for (temp=1000; temp; temp--);
}

//**************************************************************************
//* LCD initialization
//**************************************************************************
//* Calling arguments:
//* char mode1 : display mode (number of lines and character size)
//* char mode2 : display mode (cursor and display state)
//**************************************************************************
void LCD_init(char mode1, char mode2)
{
	char aux;
	// Configure the pins as outputs
	LCD_ENABLE_DIR = 1;
	LCD_RS_DIR = 1;
	LCD_D4_DIR = 1;
	LCD_D5_DIR = 1;
	LCD_D6_DIR = 1;
	LCD_D7_DIR = 1;
	// Set the LCD data pins to zero
	LCD_D4 = 0;
	LCD_D5 = 0;
	LCD_D6 = 0;
	LCD_D7 = 0;
	LCD_RS = 0;
	LCD_ENABLE = 0;       // LCD enable = 0
	LCD_delay_ms(15);
	// LCD 4-bit mode initialization sequence
	// send three times 0x03 and then 0x02 to finish configuring the LCD
	for(aux=0;aux<3;++aux)
	{		
	  LCD_send_nibble(3);
	  LCD_delay_ms(5);
	}
	LCD_send_nibble(2);
	// Now send the LCD configuration data
	LCD_send_byte(0,0x20 | mode1);
	LCD_send_byte(0,0x08 | mode2);
	lcd_mode = 0x08 | mode2;
	LCD_send_byte(0,1);
	LCD_send_byte(0,6);
}

//**************************************************************************
//* LCD cursor position set
//**************************************************************************
//* Calling arguments:
//* char x : column (starting by 0)
//* char y : line (0 or 1)
//**************************************************************************
void LCD_pos_xy(char x, char y)
{
  char address;
  if (y) address = LCD_SEC_LINE; else address = 0;
  address += x;
  LCD_send_byte(0,0x80|address);
}

//**************************************************************************
//* Write a character on the display
//**************************************************************************
//* Calling arguments:
//* char c : character to be written
//**************************************************************************
//* Notes :
//* \f clear the display
//* \n and \r return the cursor to line 1 column 0
//**************************************************************************
void LCD_write_char(char c)
{
  switch (c)
	{
	  case '\f' :	
	    LCD_send_byte(0,1);
	    LCD_delay_ms(5);
	    break;
	  case '\n' :
	  case '\r' :	
	    LCD_pos_xy(0,1);
	    break;
	  default:
	    LCD_send_byte(1,c);
   }
}

//**************************************************************************
//* Write a string on the display
//**************************************************************************
//* Calling arguments:
//* char *c : pointer to the string
//**************************************************************************
void LCD_write_string (char *c)
{
	while (*c)
	{
	  LCD_write_char(*c);
	  c++;
	}
}

//**************************************************************************
//* Turn the display on
//**************************************************************************
void LCD_display_on(void)
{
	lcd_mode |= 4;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn the display off
//**************************************************************************
void LCD_display_off(void)
{
	lcd_mode &= 0b11111011;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn the cursor on
//**************************************************************************
void LCD_cursor_on(void)
{
  lcd_mode |= 2;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn the cursor off
//**************************************************************************
void LCD_cursor_off(void)
{
	lcd_mode &= 0b11111101;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn on the cursor blink function
//**************************************************************************
void LCD_cursor_blink_on(void)
{
	lcd_mode |= 1;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn off the cursor blink function
//**************************************************************************
void LCD_cursor_blink_off(void)
{
	lcd_mode &= 0b11111110;
	LCD_send_byte (0,lcd_mode);
}

// DEMO9S08QG8 - FLASH writing/erasing/reading example

#include <hidef.h> 	      /* for EnableInterrupts macro */
#include "derivative.h" 	/* include peripheral declarations */
#include "hcs08.h" 	      // This is our definition file!
#include "doonstack.h"

#define BUSCLK  8000000
#define vFCDIV  (BUSCLK/200000-1)

char fdata, operation;
unsigned int faddress;

// Write-protect addresses ranging from 0xE200 up to 0xFFFF
const byte NVPROT_INIT @0x0000FFBD = 0xE0;

// MCU innitialization
void mcu_init(void)
{
  SOPT1 = bBKGDPE;        	// Enable debug pin
  ICSSC = NV_FTRIM;       	// configure FTRIM value
  ICSTRM = NV_ICSTRM;     	// configure TRIM value
  ICSC2 = 0;              	// ICSOUT = DCOOUT / 1
  // BUSCLK is now 8 MHz
  FCDIV = vFCDIV;         	// set FCLK divider (FCLK = 200kHz)
}

#pragma inline
// Read the content of a memory address
char flash_read(unsigned int address)
{
  unsigned char *pointer;
  pointer = (char*) address;
  return (*pointer);
}

// Write a byte into the specified FLASH address
char flash_write(unsigned int address, unsigned char data)
{
  unsigned char *pointer;
  pointer = (char*) address;
  FlashProg(pointer,data);  	// call the FLASH programming function
  if (FSTAT_FACCERR) data=1; else data=0;
  if (FSTAT_FPVIOL) data|=2;
  return(data);
}

// Erase a sector of the FLASH memory
unsigned char flash_sector_erase(unsigned int address)
{
  unsigned char *pointer, res;
  pointer = (char*) address;
  FlashErase(pointer);
  if (FSTAT_FACCERR) res=1; else res=0;
  if (FSTAT_FPVIOL) res|=2;
  return(res);
}

void main(void) 
{
  mcu_init();
  fdata = 0;
  faddress = 0xE000;
  operation = 0;
  while (1)
  {
    switch (operation)
    {
      case 1: // sector erase
        fdata = flash_sector_erase(faddress);
        operation = 0;      
        break;
      case 2: // write operation
        fdata = flash_write(faddress,fdata);
        operation = 0;
        break;
      case 3: // read operation
        fdata = flash_read(faddress);
        operation = 0;
        break;
    }
  }
}

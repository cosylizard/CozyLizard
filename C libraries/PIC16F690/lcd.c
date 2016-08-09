/*
* LCD interface example
* Uses routines from delay.c
* This code will interface to a standard LCD controller
* like the Hitachi HD44780. It uses it in 4 bit mode, with
* the hardware connected as follows (the standard 14 pin
* LCD connector is used):
*
* LCD_DATA bits, defined by D4..7, are connected to the LCD data bits 4-7 (high nibble)
* PORTA bit 3 is connected to the LCD RS input (register select)
* PORTA bit 1 is connected to the LCD EN bit (enable)
*
* To use these routines, set up the port I/O (TRISA, TRISD) then
* call lcd_init(), then other routines as required.
*
*/


#include <pic.h>
#include "lcd.h"
#include "delay.h"

//hardware configuration - change hardware configuration here.
#define LCD_RS (1<<1) //if move the pins to a different port, need to revise TRISA as well
#define LCD_RW (1<<2)
#define LCD_EN (1<<3)
#define init_ctrl TRISC &= ~(LCD_RS | LCD_RW | LCD_EN)  //RA1..3 as digital output

//#define LCD_RS (1<<1);
//#define LCD_RW (1<<2);
//#define LCD_EN (1<<3);

//if you change the pin out for _RS, _RW and _EN, you will need to revise the above macro accordingly

#define LCD_DATA PORTC  //led connected to portb. if move the port, need to revise TRISB in lcd_init()
#define D4   (1<<4)  //LCD's D4 connected to LCD_DATA's 7th bit
#define D5   (1<<5)  //LCD's D5 connected to LCD_DATA's 0th bit
#define D6   (1<<6)  //LCD's D6 connected to LCD_DATA's 5th bit
#define D7   (1<<7)  //LCD's D7 connected to LCD_DATA's 2nd bit
#define init_data TRISC &= ~(D4 | D5 | D6 | D7) //D4..7 bits on LCD_PORT set to digital output
//hardware configuration

void setLCD_RS(int state)
{
if (state == 1)
                {LCD_DATA |= 0b00000010;}
else            {LCD_DATA &= ~0b00000010;}
}

void setLCD_RW(int state)
{
if (state == 1)
                {LCD_DATA |= 0b00000100;}
else            {LCD_DATA &= ~0b00000100;}
}

void setLCD_EN(int state)
{
if (state == 1)
                { LCD_DATA |= 0b00001000;}
else            {LCD_DATA &= ~0b00001000;}
}

#define LCD_STROBE() ((setLCD_EN(1)),(setLCD_EN(0)))

/* write a byte to the LCD in 4 bit mode */

void
lcd_write(unsigned char c)
{
DelayUs(40);
// LCD_DATA = ( ( c >> 4 ) & 0x0F );  //write out the highest four bits
if (c & 0b10000000) LCD_DATA |= D7; else LCD_DATA &= ~D7;
if (c & 0b01000000) LCD_DATA |= D6; else LCD_DATA &= ~D6;
if (c & 0b00100000) LCD_DATA |= D5; else LCD_DATA &= ~D5;
if (c & 0b00010000) LCD_DATA |= D4; else LCD_DATA &= ~D4;
LCD_STROBE();
// LCD_DATA = ( c & 0x0F );    //write out the lowest four bits
if (c & 0b00001000) LCD_DATA |= D7; else LCD_DATA &= ~D7;
if (c & 0b00000100) LCD_DATA |= D6; else LCD_DATA &= ~D6;
if (c & 0b00000010) LCD_DATA |= D5; else LCD_DATA &= ~D5;
if (c & 0b00000001) LCD_DATA |= D4; else LCD_DATA &= ~D4;
LCD_STROBE();
}

/*
*  Clear and home the LCD
*/

void
lcd_clear(void)
{
setLCD_RS(0);
lcd_write(0x1);
DelayMs(2);
}

/* write a string of chars to the LCD */

void
lcd_puts(const char * s)
{
setLCD_RS(1); // write characters
while(*s)
 lcd_write(*s++);
//lcd_write(50);
}

void lcd_puti(int i)
{
    setLCD_RS(1);
    lcd_write((i % 10) + 48);
}

/* write one character to the LCD */

void
lcd_putch(char c)
{
setLCD_RS(1); // write characters
lcd_write( c );
}


/*
* Go to the specified position
*/

void
lcd_goto(unsigned char pos)
{
setLCD_RS(0);
lcd_write(0x80+pos);
}

void
lcd_cmd(unsigned char c)
{
    setLCD_RS(0);
    lcd_write(c);
}

/* initialise the LCD - put into 4 bit mode */
void
lcd_init()
{
char init_value;

// ADCON1 = 0x06; // Disable analog pins on PORTA
//CMCON=0x07;  //turn off the comparators on porta;
init_value = 0x3;
//init_ctrl;  //RA1..3 as digital output
//init_data; //D4..7 bits on LCD_PORT set to digital output
TRISC = 0;
setLCD_RS(0);
setLCD_EN(0);
setLCD_RW(0);

DelayMs(15); // wait 15mSec after power applied,
// LCD_DATA  = init_value;
LCD_DATA = (LCD_DATA & (~(D4 | D5 | D6 | D7))) + (D4 | D5);
LCD_STROBE();
DelayMs(5);
LCD_STROBE();
DelayUs(200);
LCD_STROBE();
DelayUs(200);
// LCD_DATA = 2; // Four bit mode. implementation dep
LCD_DATA = (LCD_DATA & (~(D4 | D5 | D6 | D7))) + D5;
LCD_STROBE();

lcd_write(0x28); // Set interface length
lcd_write(0xF); // Display On, Cursor On, Cursor Blink
lcd_clear(); // Clear screen
lcd_write(0x6); // Set entry Mode
}


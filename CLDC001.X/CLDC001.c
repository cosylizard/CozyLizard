/*
 *CLDC001 Display Controller (4x20 HD44780 LCD Display) v0.2
 *(c) Cosy Lizard - 03 June 2015
 * line addresses   0x00 = line 1
 *                  0x40 = line 2
 *                  0x14 = line 3
 *                  0x54 = line 4
 * Modified 23 Feb 2016 - Changed from RS232 to CLDTP1 protocol
 */


#define _LEGACY_HEADERS //Added for compiler versions 9.81+
#include <htc.h>
#include "lcd.h"
#include "usart.h"
#include "CLDTP1.h"

void pause( unsigned short usvalue );		//Establish pause routine function

__CONFIG (INTIO & WDTDIS & MCLRDIS & UNPROTECT);

#define _XTAL_FREQ 8000000
#define ASCIIConvert 0x30  //add to integer to display correspondig ASCII code

unsigned char b0;


void DisplayData(int data) //display 3 digit number
{
    lcd_putch((data/100)%10 + 48);
    lcd_putch((data/10)%10 + 48);
    lcd_puts(".");
    lcd_putch((data)%10 + 48);
}

void RecieveSensorData()
{
    //recieve data packet and assign to variables
    unsigned char ControllerID = onewire_listen(0xF0);
    unsigned char Temp1H = onewire_listen(0xF0);
    unsigned char Temp1L = onewire_listen(0xF0);
    unsigned char Temp2H = onewire_listen(0xF0);
    unsigned char Temp2L = onewire_listen(0xF0);
    unsigned char Temp3H = onewire_listen(0xF0);
    unsigned char Temp3L = onewire_listen(0xF0);
    unsigned char Temp4H = onewire_listen(0xF0);
    unsigned char Temp4L = onewire_listen(0xF0);
    unsigned char HumidH = onewire_listen(0xF0);
    unsigned char HumidL = onewire_listen(0xF0);

    //combine high and low bytes
    int Temp1 = (Temp1H<<8) + Temp1L;
    int Temp2 = (Temp2H<<8) + Temp2L;
    int Temp3 = (Temp3H<<8) + Temp3L;
    int Temp4 = (Temp4H<<8) + Temp4L;
    int Humid = (HumidH<<8) + HumidL;

    //display
    lcd_clear();
    lcd_goto(0x0);

    lcd_puts("Controller ");
    lcd_putch(ControllerID + 48);

    //display temperature
    lcd_goto(0x40);
    lcd_puts("Tmp1 ");
    DisplayData(Temp1);

    lcd_puts(" Tmp2 ");
    DisplayData(Temp2);

    lcd_goto(0x14);
    lcd_puts("Tmp3 ");
    DisplayData(Temp3);

    lcd_puts(" Tmp4 ");
    DisplayData(Temp4);

    //display humidity
    lcd_goto(0x54);
    lcd_puts("Humidity ");
    DisplayData(Humid);
}

void
main(void)
{
unsigned char  input;

ANSEL = 0;		// Set PORTA all Digital
ANSELH = 0;
CM1CON0 = 0;	// Comparator 1 off
CM2CON0 = 0;	// Comparator 2 off
OSCCON = 0b01110110;

TRISA = 0;		// All PORTA Outputs
TRISB = 0;
TRISC = 0;		// All PORTC Outputs
PORTB = 0;

lcd_init();		// Initialize LCD Display
onewire_init();
//init_comms();

lcd_clear();
lcd_goto(0);
lcd_puts("    Cosy Lizard");
lcd_goto(0x40);
lcd_puts("Initialising...");

__delay_ms(1000);  //delay 1 second to allow other devices on the bus to start up
while (1)
	{
            input=onewire_listen(0xF0);  //wait for command

            if (input == 0x01) //command 0x01 recieve sensor data
                RecieveSensorData();
	}
}
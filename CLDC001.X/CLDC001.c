/*
 *CLDC001 Display Controller (4x20 HD44780 LCD Display) v0.3
 *(c) Cosy Lizard - 03 June 2015
 * line addresses   0x00 = line 1
 *                  0x40 = line 2
 *                  0x14 = line 3
 *                  0x54 = line 4
 * Modified 23 Feb 2016 - Changed from RS232 to CLDTP1 protocol
 * Modified 20 Aug 2016 - Change Page command added
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
int DataArray[4][5];
unsigned char DisplayPage = 1;  //sensor page currently being displayed

void DisplayData(int data) //display 3 digit number
{
    lcd_putch((data/100)%10 + 48);
    lcd_putch((data/10)%10 + 48);
    lcd_puts(".");
    lcd_putch((data)%10 + 48);
}


void RefreshDisplay()
{
    //display
    lcd_clear();
    lcd_goto(0x0);

    lcd_puts("Controller ");
    lcd_putch(DisplayPage + 48);

    //display temperature
    lcd_goto(0x40);
    lcd_puts("Tmp1 ");
    DisplayData(DataArray[DisplayPage][0]);

    lcd_puts(" Tmp2 ");
    DisplayData(DataArray[DisplayPage][1]);

    lcd_goto(0x14);
    lcd_puts("Tmp3 ");
    DisplayData(DataArray[DisplayPage][2]);

    lcd_puts(" Tmp4 ");
    DisplayData(DataArray[DisplayPage][3]);

    //display humidity
    lcd_goto(0x54);
    lcd_puts("Humidity ");
    DisplayData(DataArray[DisplayPage][4]);
}

void ReceivePage()
{
    //get page number and assign page vaiable
    DisplayPage = onewire_listen(0xF0);

    RefreshDisplay();
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
//    int Temp1 = (Temp1H<<8) + Temp1L;
//    int Temp2 = (Temp2H<<8) + Temp2L;
//    int Temp3 = (Temp3H<<8) + Temp3L;
//    int Temp4 = (Temp4H<<8) + Temp4L;
//    int Humid = (HumidH<<8) + HumidL;

    //load data into DataArray
    DataArray[ControllerID][0] = (Temp1H<<8) + Temp1L;
    DataArray[ControllerID][1] = (Temp2H<<8) + Temp2L;
    DataArray[ControllerID][2] = (Temp3H<<8) + Temp3L;
    DataArray[ControllerID][3] = (Temp4H<<8) + Temp4L;
    DataArray[ControllerID][4] = (HumidH<<8) + HumidL;

    RefreshDisplay();
}


void main(void)
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
            if (input == 0x02) //command 0x02 switch page
                ReceivePage();
	}
}
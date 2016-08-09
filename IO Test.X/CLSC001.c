/* 
 *CLCS001 Sensor Controller (temperature and humidity) v0.1
 *(c) Cosy Lizard - 27 Feb 2015
 *
 * SENSOR ID = 0x00
 * OUTPUT - 16 bit value representing requested value
 * ERROR VALUES -   0xFFFF = Sensor Timeout
 *                  0xFFFE = Sensor Error
 *                  0xFFFD = Invalid Command
 *
 * DEVELOPMENT LIST
 * Include support for DS18b20 sensors
 * Include automatic identification of sensor type
 */

#include "usart.h"
#include <htc.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "dht22.h"
#include "ds18b20.h"

#define _XTAL_FREQ 8000000
__CONFIG(WDTE_OFF);

void interrupt isr(void)
{
    if(TMR1IF =1) //sensor timeout triggered
    {
        TMR1H = 0x0;
        TMR1L = 0x0;
        DHTTimeOut = 1;
        TMR1IF = 0;
    }
}

void main()
{
unsigned char input;

OSCCON = 0b01110110;
init_comms();

ANSEL = 0;
ANSELH = 0;
PORTC = 0;
TRISC = 0b00000001;

DHTTimeOut = 0;

////ds18b20 test routine
//while(1)
//{
//    __delay_ms(10000);
//    ds18b20_GetTemperature(0b00000001);
//}

while(1)
{
    input=getch();//wait for command

    if (input==0)
    {
        putch(0x0);
    }
    else if (input==1) //read temp from sensor 1
    {
        int temperature = GetTemperature(0b00000001);

        putch(temperature >> 8);
        putch(temperature);
    }
    else if (input==2) //read humidity from sensor 1
    {
        int humidity = GetHumidity(0b00000001);

        putch(humidity >> 8);
        putch(humidity);
    }
    else if (input==3) //read temp from sensor 2
    {
        int temperature = GetTemperature(0b00000010);

        putch(temperature >> 8);
        putch(temperature);
    }
    else if (input==4) //read humidity from sensor 2
    {
        int humidity = GetHumidity(0b00000010);

        putch(humidity >> 8);
        putch(humidity);
    }
    else if (input==5) //read temp from sensor 3
    {
        int temperature = GetTemperature(0b00000100);

        putch(temperature >> 8);
        putch(temperature);
    }
    else if (input==6) //read humidity from sensor 3
    {
        int humidity = GetHumidity(0b00000100);

        putch(humidity >> 8);
        putch(humidity);
    }
    else if (input==7) //read temp from sensor 4
    {
        int temperature = GetTemperature(0b00001000);

        putch(temperature >> 8);
        putch(temperature);
    }
    else if (input==8) //read humidity from sensor 4
    {
        int humidity = GetHumidity(0b00001000);

        putch(humidity >> 8);
        putch(humidity);
    }
//    else if (input==9) //read temperature from ds18b20
//    {
//        int temperature = ds18b20_GetTemperature(0b00000001);
//
//        //putch(temperature);
//    }
        else
    {
        putch(0xFF);
        putch(0xFD);
    }
        
}
}


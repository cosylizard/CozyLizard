/* 
 *CLCS001 Sensor Controller (temperature and humidity) v0.4
 *(c) Cosy Lizard - 23 February 2016
 *
 * SENSOR ID = 0x00
 * OUTPUT - 16 bit value representing requested value
 * ERROR VALUES -   0xFFFF = Sensor Error
 *                  0xFFFE = Sensor Timeout
 *                  0xFFFD = Invalid Command
 *                  0xFFFC = Measurement not valid with this sensor
 *                  0xFFFB = No sensor on this channel
 *                  0xFFFA = Unknown Sensor Type
 *                  0xFFF9 = Controller Initiated Successfully
 * DEVELOPMENT LIST 
 * Fixed-Issue with timeouts when multiple ds18b20 connected (work around with a second attempt, what is the real issue???)
 * v0.4 - changed from USART to CLDTP1
 */

#include "usart.h"
#include <htc.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "dht22.h"
#include "ds18b20.h"
#include "CLDTP1.h"

#define _XTAL_FREQ 8000000
__CONFIG(WDTE_OFF);

//variables holding sensor types 0 = None, 1 = DHT22, 2 = DS18b20
unsigned char Sensor1;
unsigned char Sensor2;
unsigned char Sensor3;
unsigned char Sensor4;


void interrupt isr(void)
{
    if(TMR1IF =1) //sensor timeout triggered
    {
        TMR1H = 0x0;
        TMR1L = 0x0;
        DHTTimeOut = 1;
        DS18b20TimeOut = 1;
        TMR1IF = 0;
    }
}

unsigned char IdentifySensor(unsigned char bitmap)
{
    if (GetTemperature(bitmap) == 65534) //check for DHT22
    {
        if (ds_gettemperature(bitmap) == 65534)//check for ds18b20
            return 0; //no sensor detected
        else
            return 2; //ds18b20 detected
    }
    else
        return 1; //DHT22 detected
}

void AutoDetectSensors()
{
    Sensor1 = IdentifySensor(0b00000001);
    Sensor2 = IdentifySensor(0b00000010);
    Sensor3 = IdentifySensor(0b00000100);
    Sensor4 = IdentifySensor(0b00001000);

//    putch(0xFF);//report initialisation complete
//    putch(0xF9);
    __delay_ms(10);
    onewire_sendpacket(0x00,0x10,0xFF);
    __delay_ms(10);
    onewire_sendpacket(0x00,0x10,0xF9);

}

void ReturnTemperature(int SensorType, unsigned char bitmap)
{
    if (SensorType == 0) //no sensor detected, return FFFB
    {
//        putch(0xFF);
//        putch(0xFB);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0xFF);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0xFB);
    }
    else if (SensorType == 1) //return value from DHT22
    {
        int temperature = GetTemperature(bitmap);

        if (temperature == 0xFFFE)  //check for timeout and try again
        {
            temperature = GetTemperature(bitmap);
        }

//        putch(temperature >> 8);
//        putch(temperature);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,temperature >> 8);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,temperature);
    }
    else if (SensorType == 2) //return value from DS18b20
    {
        int temperature = ds_gettemperature(bitmap);

        if (temperature == 0xFFFE)  //check for timeout and try again
        {
            temperature = ds_gettemperature(bitmap);
        }

//        putch(temperature >> 8);
//        putch(temperature);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,temperature >> 8);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,temperature);
    }   
    else //return unknown sensor type error code
    {
//        putch(0xFF);
//        putch(0xFA);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0xFF);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0xFA);
    }

}

void ReturnHumidity(int SensorType, unsigned char bitmap)
{

    if (SensorType == 0) //no sensor detected, return FFFB
    {
//        putch(0xFF);
//        putch(0xFB);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0xFF);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0xFB);
    }
    else if (SensorType == 1) //return humidity value for DHT22
    {
        int humidity = GetHumidity(bitmap);

        if (humidity == 0xFFFE)  //check for timeout and try again
        {
            humidity = GetHumidity(bitmap);
        }
        
//        putch(humidity >> 8);
//        putch(humidity);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,humidity >> 8);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,humidity);
    }
    else if (SensorType == 2) //measurement not valid for ds18b20
    {
//        putch(0xFF);
//        putch(0xFC);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0xFF);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0xFC);
    }
        else //return unknown sensor type error code
    {
//        putch(0xFF);
//        putch(0xFA);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0xFF);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0xFA);
    }
}

void main()
{
unsigned char input;
OSCCON = 0b01110110;

//__delay_ms(5000);

//init_comms();



ANSEL = 0;
ANSELH = 0;
PORTB = 0;
TRISB = 0;
PORTC = 0;
TRISC = 0b00001111;

onewire_init();
__delay_ms(1000);  //allow a second for other devices on the bus to start up

DHTTimeOut = 0;

//AutoDetectSensors();  //auto detect connected sensors - disabled auto-detect to prevent 0 return on init problem

//while(1)
//{
//    __delay_us(20);
//    RB5 = 1;
//__delay_us(20);
//    RB5 = 0;
//
//}


while(1)
{
    //input=getch();//wait for command
    input = onewire_listen(0x10);

    if (input==0) //identify controller type
    {
        //putch(0x0);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0x00);
    }
    else if (input==1) //read temp from sensor 1
    {
        ReturnTemperature(Sensor1, 0b00000001);
    }
    else if (input==2) //read humidity from sensor 1
    {
        ReturnHumidity(Sensor1, 0b00000001);
    }
    else if (input==3) //read temp from sensor 2
    {
        ReturnTemperature(Sensor2, 0b00000010);
    }
    else if (input==4) //read humidity from sensor 2
    {
        ReturnHumidity(Sensor2, 0b00000010);
    }
    else if (input==5) //read temp from sensor 3
    {
        ReturnTemperature(Sensor3, 0b00000100);
    }
    else if (input==6) //read humidity from sensor 3
    {
        ReturnHumidity(Sensor3, 0b00000100);
    }
    else if (input==7) //read temp from sensor 4
    {
        ReturnTemperature(Sensor4, 0b00001000);
    }
    else if (input==8) //read humidity from sensor 4
    {
        ReturnHumidity(Sensor4, 0b00001000);
    }
    else if (input==0xFD) //return firmware version
    {
        //putch(0x00);
        //putch(0x03);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0x00);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0x04);
    }
    else if (input==0xFF) //reinitalise controller
    {
        AutoDetectSensors();
    }
    else //return invalid command error code
    {
//        putch(0xFF);
//        putch(0xFD);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0xFF);
        __delay_ms(10);
        onewire_sendpacket(0x00,0x10,0xFD);
    }
}
}


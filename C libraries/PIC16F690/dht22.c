//DHT22 Temperature and Humidity Sensor Driver
//Compatibility - HITECH C, PIC16F690
//Version 1.1
//(c) Cosy Lizard - 27 Feb 2015
//Currently setup to take inputs from PORTC0-3 and thesse pins must be reserved exclusivly for sensor IO
//DataBitMap is a bit map referring to the IO pin form which to access the DHT22
//
//Timeout - Timer1 is used to act as a time out for sensor readings, the following must be added to the main interupt code
//
//Outputs - 16bit value representing temperature or humidity output
//Errors - 0xFFFF = parity error, 0xFFFE = timeout
//
//void interrupt isr(void)
//{
//    if(TMR1IF =1)
//    {
//        TMR1H = 0x0;
//        TMR1L = 0x0;
//        DHTTimeOut = 1;
//        TMR1IF = 0;
//    }
//}

#include <htc.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "dht22.h"

char HumidH;
char HumidL;
char TempH;
char TempL;
char CheckSum;
char DataBitMap;
bit Result;
bit DHT_Error;

bit ReadBit()
{
    //while signal low
    while((DataBitMap&PORTC)==0)
    {}
    __delay_us(30);
    //read data bit value from sensor
    if((DataBitMap&PORTC)>0)
    {
        Result = 1;
    }
    else
    {
        Result = 0;
    }

    //while signal high
    while((DataBitMap&PORTC)>0)
    {}
    return Result;
}

void GetData()
{
        //release line
        TRISC = 0b00001111;
        PORTC = (~DataBitMap)&PORTC;
        __delay_ms(2000);

        //send start bit
        TRISC = 0b00000000;
        PORTC = (~DataBitMap)&PORTC;
        __delay_ms(2);

        //release line
        TRISC = 0b00001111;
        
        //Setup Timeout Clock
        TMR1ON = 0;
        DHTTimeOut = 0;
        TMR1H = 0x0;
        TMR1L = 0x0;
        T1CON = 0b00110000;
        PIE1 = 0b00000001;
        INTCON = 0b11000000;
        TMR1ON = 1;

        //recieve responce sequence from sensor

        //while signal high
        while (((DataBitMap&PORTC)>0)&&!DHTTimeOut)
        {
        }

        //while signal low
        while (((DataBitMap&PORTC)==0)&&!DHTTimeOut)
        {
        }

        //while signal high
        while (((DataBitMap&PORTC)>0)&& !DHTTimeOut)
        {
        }

        TMR1ON = 0; //stop timeout clock

        if (!DHTTimeOut) //if timeout hasn't occured
        {
            for (int i = 0; i < 8; i++) //get first humidity byte
            {
                HumidH = (HumidH << 1) + ReadBit();
            }

             for (int i = 0; i < 8; i++) //get second humidity byte
            {
                HumidL = (HumidL << 1) + ReadBit();
            }

             for (int i = 0; i < 8; i++) //get first temperature byte
            {
                TempH = (TempH << 1) + ReadBit();
            }

             for (int i = 0; i < 8; i++) //get second temperature byte
            {
                TempL = (TempL << 1) + ReadBit();
            }

             for (int i = 0; i < 8; i++) //get parity byte
            {
                CheckSum = (CheckSum << 1) + ReadBit();
            }

            int Check;

            Check = ((HumidH + HumidL + TempH + TempL) & 0xFF); //calculate checksum

            if (Check == CheckSum)  //set error flag if checksum doesn't match
            {
                DHT_Error = 0;
            }
            else
            {
                DHT_Error = 1;
            }
        }
}

unsigned int GetTemperature(char databitmap)
{
    unsigned int temperature;

    DataBitMap = databitmap;

    GetData();

    if ((!DHT_Error)&&(!DHTTimeOut)) //if error flag not set, return temperature else error code
    {
        temperature = TempL;
        temperature += (TempH << 8);
        return temperature;
    }
    else if (DHT_Error) //if parity error occured
        return 0xFFFF;
    else if (DHTTimeOut) //if timeout error occured
        return 0xFFFE;
}

unsigned int GetHumidity(char databitmap)
{
    unsigned int humidity;

    DataBitMap = databitmap;

    GetData();

    if ((!DHT_Error)&&(!DHTTimeOut))  //if error flag not set, return humidity else error code
    {
        humidity = HumidL;
        humidity += (HumidH << 8);
        return humidity;
    }
    else if (DHT_Error) //if parity error occured
        return 0xFFFF;
    else if (DHTTimeOut) //if timeout error occured
        return 0xFFFE;
}

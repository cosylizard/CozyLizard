#include <htc.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "ds18b20.h"

char DataBitMap;

char GenerateCRC(char crc, char data) //generate CRC code
{
    crc = crc ^ data;
    for (int i; i < 0; i++)
    {
        if (crc & 0x01)
            crc = (crc >> 1) ^ 0x8C;
        else
            crc >>= 1;
    }
    return crc;
}

void ds_reset()
{
    //TRISC = 0b00000001;
    TRISC = TRISC | DataBitMap;
    PORTC = 0b00000000;

    //Setup Timeout Clock
        TMR1ON = 0;
        DS18b20TimeOut = 0;
        TMR1H = 0x0;
        TMR1L = 0x0;
        T1CON = 0b00110000;
        PIE1 = 0b00000001;
        INTCON = 0b11000000;
        TMR1ON = 1;


    //start reset sequence

    //pull line low for 480us
    TRISC = 0;
    __delay_us(480);

    //release line
    //TRISC = 0b00000001;
    TRISC = TRISC | DataBitMap;
    __delay_us(50);

    //receive response signal from sensor

    //while signal high
    while (((DataBitMap&PORTC)>0)&&!DS18b20TimeOut)
    {
    }

    //while signal low
    while (((DataBitMap&PORTC)==0)&&!DS18b20TimeOut)
    {
    }

    TMR1ON = 0; //stop timeout clock

    __delay_us(5); //recovery wait
}

void ds_write(char data)
{
    for (int count = 8;count > 0; count--) //repeat for all 8 bits
    {
        char lsb = data & 0x01;  //get least significant bit

        if (lsb > 0)
        {
            lsb = 0b11111111;
        }
        else
        {
            lsb = 0b00000000;
        }

        //pull line low to begin timeslot
        TRISC=0;
        __delay_us(7);

        //transmit lsb
        TRISC = lsb & DataBitMap;
        __delay_us(55);

        //release line
        //TRISC = 0b00000001;
        TRISC = TRISC | DataBitMap;

        //recovery time
        __delay_us(5);

        //shift to next bit
        data = data>>1;
    }
    __delay_us(5); //recovery time delay

}

char ds_read()
{
    char data = 0x0;
    char databit = 0;
    for (int count = 8;count > 0;count--) //repeat for 8 bits
    {
        //pull line low to begin timeslot
        TRISC=0;
        __delay_us(6);

        //release line
        TRISC = TRISC | DataBitMap;
        __delay_us(6);

        //read databit
        if ((PORTC & DataBitMap) > 0)
        {
        databit = 1;
        }
        else
        {
            databit = 0;
        }
        

        //finish timeslot
        __delay_us(50);

        //shift latest bit into data
//        data = data >> 1;
//        if (databit > 0)
//            data += 0b10000000;

        data = (data >> 1);
        if (databit > 0)
        {
            data = (data + 0b10000000);
        }
        __delay_us(5); //recovery delay
    }
    __delay_us(5); //recovery delay
    return(data);
}

char ds_checkCRC(char databitmap)
{
    char crc = 0x00;
    DataBitMap = databitmap;

    ds_write(0xcc); //skip rom
    ds_write(0x44); //convert T

    __delay_ms(1000); //allow for conversion

    ds_reset();
    ds_write(0xcc); //skip rom

    for (int i=0; i < 8; i++)
    {
        ds_write(0xbe);
        crc = GenerateCRC(crc, ds_read());
    }

    if(ds_read() == crc)
        return 0x01;
    else
        return 0x00;
}

unsigned int ds_gettemperature(char databitmap)
{
    unsigned int result;
    unsigned int result_int;
    unsigned int result_decimals;

    DataBitMap = databitmap;

    ds_reset();
    if (!DS18b20TimeOut) //if timeout didn't occur then continue
    {
        ds_write(0xcc); //skip rom
        ds_write(0x44); //convert T

        __delay_ms(1000); //allow for conversion

        ds_reset();
        ds_write(0xcc); //skip rom
        ds_write(0xbe); //read scratchpad

        result = ds_read();
        result += (ds_read() << 8);

        //format result
        //get whole numbers
        result_int = ((result >> 4) * 10);

        //get decimal places
        result_decimals = result & 0b00001111;
        result_decimals = result_decimals * 625;
        result_decimals = result_decimals / 1000;

        result = result_int + result_decimals;
        return result;
    }
    else
    {
        return 0xFFFE;
    }
}
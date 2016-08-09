/*
 *Cosy Lizard Data Transfer Protocol (CLDTP1) v0.1
 *(c) Cosy Lizard - 13 February 2016
 * Uses the PIC USART Tx pin for transmission (RB7, pin 10)
 */


#include <htc.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "CLDTP1.h"

void onewire_write(char data)
{
    for (int count = 8;count > 0; count--) //repeat for all 8 bits
    {
        char lsb = data & 0x01;  //get least significant bit

        if (lsb > 0)
        {
            lsb = 1;
        }
        else
        {
            lsb = 0;
        }

        //pull line low to begin timeslot
        TransmitPin=0;
        __delay_us(7);

        //transmit lsb
        TransmitPin = lsb;
        __delay_us(55);

        //release line
        //TRISC = 0b00000001;
        TransmitPin = 1;

        //recovery time
        __delay_us(5);

        //shift to next bit
        data = data>>1;
    }
    __delay_us(5); //recovery time delay
}

unsigned char onewire_read()
{
    char data = 0x0;
    char databit = 0;
    for (int count = 8;count > 0;count--) //repeat for 8 bits
    {
        __delay_us(30); //wait until middle of bit time slot

        //read databit
        if ((PORTB & 0b10000000) > 0)
        {
            databit = 1;
        }
        else
        {
            databit = 0;
        }

        //shift latest bit into data

        data = (data >> 1);
        if (databit > 0)
        {
            data = (data + 0b10000000);
        }


       //wait until end of timeslot
        while((PORTB & 0b10000000) == 0)
        {}

        if (count > 1) //if not the last bit
        {
            while((PORTB & 0b10000000) > 0) //wait for falling edge and start of next bit
            {}
        }

    }
    return data;
}

unsigned char onewire_listen(char address)
{
    unsigned char RxAddress;
    unsigned char TxAddress;
    unsigned char Payload;
    int message_recieved = 0;

    while(message_recieved == 0)
    {
        while((0b10000000&PORTB)>0) //wait while line is high
        {}

        //recieve RxAddress
        RxAddress = onewire_read();

        //recieve TxAddress
        __delay_us(10);
        while((0b10000000&PORTB)>0) //wait while line is high
        {}
        TxAddress = onewire_read();

        //recieve payload
        __delay_us(10);
        while((0b10000000&PORTB)>0) //wait while line is high
        {}
        Payload = onewire_read();

        if(RxAddress == address) //check Rxaddress, if matches device, return payload
        {
            message_recieved = 1;
            return Payload;
        }
    }
}

void onewire_sendpacket(char RxAddress, char TxAddress, char payload)
{
    onewire_write(RxAddress); //send reciever address
    __delay_us(10); //delay between bytes
    onewire_write(TxAddress); //send transmitter address
    __delay_us(10);
    onewire_write(payload);
}

void onewire_init()
{
    TransmitPin = 1;
}
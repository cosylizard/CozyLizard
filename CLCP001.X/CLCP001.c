/*
 *CLCP001 Central Processor v0.2
 *(c) Cosy Lizard - 06 October 2015
 *v0.2 12 July 2016 - Adjustable timeout added controller listen
 */
#define _LEGACY_HEADERS //Added for compiler versions 9.81+
#include <htc.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "math.h"
#include "usart.h"
#include "CLDTP1.h"

#define _XTAL_FREQ 8000000
__CONFIG (INTIO & WDTDIS & MCLRDIS & UNPROTECT);

unsigned char SensorsArray[4];

void interrupt isr(void)
{
    if(TMR1IF =1) //sensor timeout triggered
    {
        TimeoutOverflowCounter++; //increment overflow counter and reset timer
        TMR1H = 0x0;
        TMR1L = 0x0;
        TMR1IF = 0;

        if (TimeoutOverflowCounter >= TimeoutMultiplier) //if overflow counter = multiplier then set timeout flags
        {
            IDTimeout = 1;
        }
        else //else reset and restart timer
        {
            TMR1ON = 1;
        }
    }
}

void IdentifyConnectedControllers()
{
    for (int i = 0; i < 4; i++) //loop through sensor channels
    {
        unsigned char NextAddress = 0x10 + i;

        onewire_sendpacket(NextAddress,0x00,0x00); //send 'who are you message'

        BreakOnTimeout = 1;
        TimeoutMultiplier = 1;
        SensorsArray[i] =  onewire_listen(0x00);//save controller type to array with timeout enabled
        BreakOnTimeout = 0;
    }
}

void InitialiseSensors()
{
    unsigned char SensorAddress;
    //initialise all known sensors
    for (int i = 0; i < 4; i++)
    {
        SensorAddress = 0x10 + i;

        if (SensorsArray[i] != 0xFF)  //if sensor is known on port
        {
            //send init command and wait for confimation
            onewire_sendpacket(SensorAddress,0x00,0xFF);
            onewire_listen(0x00);
            onewire_listen(0x00);
        }
    }
}

unsigned int CalculateAverageHumidity(unsigned char HumidityArray[4][2])
{
    unsigned int HumidSensorCount;
    HumidSensorCount = 0;

    unsigned int HumiditySum;
    HumiditySum = 0;

    for (int i = 0; i < 4; i ++) //loop through all sensor responses
    {
        if ((HumidityArray[i][0]) != 0xFF) //if returned humidity is not an invalid code
        {
            HumidSensorCount++;  //increment sensor count
            HumiditySum += ((HumidityArray[i][0]<<8) + HumidityArray[i][1]);
        }
    }

    if (HumidSensorCount > 0) //if more than 1 humidity result has been returned, return average value
    {
        return HumiditySum / HumidSensorCount;
        //return HumidSensorCount;
    }
    else
    {
        return 0x00;
    }

}

int main() {

ANSEL = 0;		// Set PORTA all Digital
ANSELH = 0;
CM1CON0 = 0;	// Comparator 1 off
CM2CON0 = 0;	// Comparator 2 off
OSCCON = 0b01110110;

TRISA = 0;  
PORTA = 0;
TRISB = 0;
PORTB = 0;


onewire_init();
__delay_ms(10000); //allow start up time of peripherals


//identify connected controllers
IdentifyConnectedControllers();

//initiate sensors  -- Disabled, auto initialisatioin should happen on the controllers
InitialiseSensors();

while (1)
{
    for (int i = 0;i < 4; i++)
    {

        unsigned char SensorAddress;  //set address of sensor to be read
        SensorAddress = 0x10 + i;

        if (SensorsArray[i] != 0xFF)
        {
            //setup listen timeout
            BreakOnTimeout = 1;
            TimeoutMultiplier = 8;

            //variables to hold sensor data
            unsigned char controllerId;
            unsigned char Temp1L;
            unsigned char Temp1H;
            unsigned char Humid1L;
            unsigned char Humid1H;
            unsigned char Temp2L;
            unsigned char Temp2H;
            unsigned char Humid2L;
            unsigned char Humid2H;
            unsigned char Temp3L;
            unsigned char Temp3H;
            unsigned char Humid3L;
            unsigned char Humid3H;
            unsigned char Temp4L;
            unsigned char Temp4H;
            unsigned char Humid4L;
            unsigned char Humid4H;


            //get information from sensors if timeout hasn't expired
            //if the timeout has previously been triggered, routine will drop out of the loop

            if (IDTimeout == 0)  //drop out of loop if timout has occured
            {
                __delay_ms(10);
                onewire_sendpacket(SensorAddress,0x00,0x00);
                controllerId = onewire_listen(0x00);
            }
            else
                continue;

            if (IDTimeout == 0) //drop out of loop if timout has occured
            {
                __delay_ms(10);
                onewire_sendpacket(SensorAddress,0x00,0x01);
                Temp1L = onewire_listen(0x00);
                __delay_ms(2);
                Temp1H = onewire_listen(0x00);
            }
            else
                continue;

            if (IDTimeout == 0)  //drop out of loop if timout has occured
            {
                __delay_ms(10);
                onewire_sendpacket(SensorAddress,0x00,0x02);
                Humid1L = onewire_listen(0x00);
                __delay_ms(2);
                Humid1H = onewire_listen(0x00);
            }
            else
                continue;

            if (IDTimeout == 0)  //drop out of loop if timout has occured
            {
                __delay_ms(10);
                onewire_sendpacket(SensorAddress,0x00,0x03);
                Temp2L = onewire_listen(0x00);
                __delay_ms(2);
                Temp2H = onewire_listen(0x00);
            }
            else
                continue;

            if (IDTimeout == 0)  //drop out of loop if timout has occured
            {
                __delay_ms(10);
                onewire_sendpacket(SensorAddress,0x00,0x04);
                Humid2L = onewire_listen(0x00);
                __delay_ms(2);
                Humid2H = onewire_listen(0x00);
            }
            else
                continue;

            if (IDTimeout == 0)  //drop out of loop if timout has occured
            {
                __delay_ms(10);
                onewire_sendpacket(SensorAddress,0x00,0x05);
                Temp3L = onewire_listen(0x00);
                __delay_ms(2);
                Temp3H = onewire_listen(0x00);
            }
            else
                continue;

            if (IDTimeout == 0)  //drop out of loop if timout has occured
            {
                __delay_ms(10);
                onewire_sendpacket(SensorAddress,0x00,0x06);
                Humid3L = onewire_listen(0x00);
                __delay_ms(2);
                Humid3H = onewire_listen(0x00);
            }
            else
                continue;

            if (IDTimeout == 0)  //drop out of loop if timout has occured
            {
                __delay_ms(10);
                onewire_sendpacket(SensorAddress,0x00,0x07);
                Temp4L = onewire_listen(0x00);
                __delay_ms(2);
                Temp4H = onewire_listen(0x00);
            }
            else
                continue;

            if (IDTimeout == 0)  //drop out of loop if timout has occured
            {
            __delay_ms(10);
            onewire_sendpacket(SensorAddress,0x00,0x08);
            Humid4L = onewire_listen(0x00);
            __delay_ms(2);
            Humid4H = onewire_listen(0x00);
            }
            else
                continue;

            __delay_ms(10);

            //tidyup timeout options
            BreakOnTimeout = 0;
            TimeoutMultiplier = 1;

            //calculate average humidity

            unsigned char Humidity[4][2];
            unsigned int Humid;
            unsigned char HumidL;
            unsigned char HumidH;


            Humidity[0][0] = Humid1L;
            Humidity[1][0] = Humid2L;
            Humidity[2][0] = Humid3L;
            Humidity[3][0] = Humid4L;

            Humidity[0][1] = Humid1H;
            Humidity[1][1] = Humid2H;
            Humidity[2][1] = Humid3H;
            Humidity[3][1] = Humid4H;

            Humid = CalculateAverageHumidity(Humidity);
            HumidL = (Humid >> 8);
            HumidH = (Humid & 0xFF);
            
              //SelectChannel(7);
              onewire_sendpacket(0xF0,0x00,0x01);  //send display command
              __delay_ms(10);
              onewire_sendpacket(0xF0,0x00,i);  //send display command
              __delay_ms(10);
              onewire_sendpacket(0xF0,0x00,Temp1L);
              __delay_ms(10);
              onewire_sendpacket(0xF0,0x00,Temp1H);
              __delay_ms(10);
              onewire_sendpacket(0xF0,0x00,Temp2L);
              __delay_ms(10);
              onewire_sendpacket(0xF0,0x00,Temp2H);
              __delay_ms(10);
              onewire_sendpacket(0xF0,0x00,Temp3L);
              __delay_ms(10);
              onewire_sendpacket(0xF0,0x00,Temp3H);
              __delay_ms(10);
              onewire_sendpacket(0xF0,0x00,Temp4L);
              __delay_ms(10);
              onewire_sendpacket(0xF0,0x00,Temp4H);
              __delay_ms(10);
              onewire_sendpacket(0xF0,0x00,HumidL);
              __delay_ms(10);
              onewire_sendpacket(0xF0,0x00,HumidH);


              __delay_ms(1000);   //change display page to sensor just received
              onewire_sendpacket(0xF0,0x00,0x02);
              __delay_ms(10);
              onewire_sendpacket(0xF0,0x00,i);

              __delay_ms(3000);
        }
    }
}

}



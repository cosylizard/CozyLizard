//****************************************************************************
//                            1wire002.c
//
//  Version 2 of the Dallas 1-wire Bus communications routines for Hi-Tech C
//  This Time Sat down and worked out my way of doing it!!
//
//
//  Author: Michael Pearce
//
//  Started: 29 June 1998
//****************************************************************************
//  24 July 1998
//    - Added ReadRom and Match Rom Commands - need definition to activate
//****************************************************************************

#define D_RiseSpace 5        // Rise Time + Minimum Space (3uS+1uS absolute min)

#ifndef DelayUs
 #include "m_delay.h"
#endif

#ifndef D_PIN_PORT
 #define D_PIN_PORT PORTA
#endif

#ifndef D_PIN_NUM
 #define D_PIN_NUM 0
#endif

#ifndef D_TRIS_PORT
 #define D_TRIS_PORT TRISA
#endif

static bit D_PIN @ BITNUM(D_PIN_PORT,D_PIN_NUM);
static bit D_TRIS @ BITNUM(D_TRIS_PORT,D_PIN_NUM);

bit  D_Presence;
bit  D_ShortCircuit;
bit  D_Error;
bit  D_Data;


//****************************************************************************
//D_Reset  -- Resets the 1-wire bus and checks for presence & short cct
//****************************************************************************
void D_Reset(void)
{
 char count=47;
 //-- Reset the status bits
 D_Error=0;

 //-- Ensure Correct port pin settings
 D_TRIS=1;
 D_PIN=0;

 //-- Start the reset Pulse
 D_TRIS=0;          //-- Pull Line Low to start reset pulse
 for(;count>0;count--)
 {
  DelayUs(10);      //-- 480uS Delay
 }
 D_TRIS=1;          //-- Release the line
 DelayUs(50);       //-- Delay to about centre of presence pulse
 DelayUs(10);
 D_Presence=D_PIN;  //-- Get Presence status 1=None 0=something there
 for(count=41;count>0;count--) //-- (480-60)uS delay till end
 {
  DelayUs(10);
 }
 D_ShortCircuit=D_PIN;  //-- Check for short circuit on bus pin.

 if(D_ShortCircuit==0)  //-- If short circuit then indicate an error
 {
  D_Error=1;
 }
 if(D_Presence==1)      //-- If no device detected indicate an error
 {
  D_Error=1;
 }
 DelayUs(D_RiseSpace);    //-- Rise time + Min Space

}
//******************END OF D_Reset


//****************************************************************************
// D_Write
//****************************************************************************
void D_Write(Data)
{
 char count=8;
 for(;count>0;count--)
 {
  D_Data= Data & 0x01;    //-- Get LSB

  //-- Write the bit to the port
  D_TRIS=0;               //-- Lower the port
  DelayUs(5);             //-- Time slot start time
  D_TRIS=D_Data;          //-- Output the data to the port
  DelayUs(50);            //-- Finish Timeslot
  DelayUs(5);

  D_TRIS=1;               //-- Ensure Release of Port Pin

  //-- Delay Between Bits
  DelayUs(D_RiseSpace);             //-- Recovery time between Bits

  //-- Prep Next Bit
  Data=Data>>1;           //-- Shift next bit into place
 }
 DelayUs(D_RiseSpace);    //-- Rise time + Min Space
}
//******************END OF D_Write

//****************************************************************************
// D_Read
//****************************************************************************
char D_Read(void)
{
 char count=8,data=0;
 for(;count>0;count--)
 {
  //-- Write the bit to the port
  D_TRIS=0;               //-- Lower the port
  DelayUs(5);             //-- Time slot start time
  D_TRIS=1;               //-- Release port for reading
  DelayUs(5);             //-- Get close to center of timeslot
  D_Data=D_PIN;           //-- Read the data bit in
  DelayUs(50);            //-- Finish the timeslot

  //-- Put data into the byte
  data = data >> 1;        //-- Shift previous bits across
  if(D_Data==1)            //-- If data was high then add High bit to data
  {
   data +=0x80;
  }
  //-- Delay Between Bits
  DelayUs(D_RiseSpace);             //-- Recovery time between Bits
 }
 DelayUs(D_RiseSpace);    //-- Rise time + Min Space
 return(data);
}
//******************END OF D_Read


//**************************************************************************
//D_MatchRom - Matches the ROM Code to device
//**************************************************************************
#ifdef _D_MATCHROM_
void D_MatchRom(const char address[])
{
 char count;
 D_Write(0x55);          //-- Match Rom
                         //-- Send the Address ROM Code.
 for(count=0;count<8;count++)
 {
  D_Write(address[count]);
 }
}
#endif
//****************** END OF D_SendAddress

//**************************************************************************
//D_ReadRom - Reads the ROM Code from a device
//**************************************************************************
#ifdef _D_READROM_
void D_ReadRom(char *address)
{
 char count;
 D_Write(0x33);          //-- Read Rom
                         //-- Read in the ROM code from the Device
 for(count=0;count<8;count++)
 {
  address[count]=D_Read();
  address++
 }
}
#endif
//****************** END OF D_ReadRom

//**************************************************************************
//
//**************************************************************************

//****************** END OF


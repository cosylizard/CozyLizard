/* 
 * File:   1-wire.h
 * Author: Dave
 *
 * Created on 13 February 2016, 08:13
 */

#define TransmitPin TRISB7
#define _XTAL_FREQ 8000000

//protoypes
void onewire_write(char data);
unsigned char onewire_read();
unsigned char onewire_listen(char address);
void onewire_sendpacket(char RxAddress, char TxAddress, char payload);
void onewire_init();

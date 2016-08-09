#define _XTAL_FREQ 8000000

unsigned int ds_gettemperature(char databitmap);
char ds_checkCRC(char databitmap);
bit DS18b20TimeOut;
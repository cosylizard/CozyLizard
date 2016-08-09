#define _XTAL_FREQ 8000000
#define DT_DATA RC0
//#define TESTBIT RC5

bit ReadBit();
bit DHTTimeOut;
char HumidH;
char HumidL;
char TempH;
char TempL;
void GetData();
unsigned int GetTemperature(char databitmap);
unsigned int GetHumidity(char databitmap);




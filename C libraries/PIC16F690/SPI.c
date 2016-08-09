#include <pic.h>

void SPIInitMaster()

{
    TRISB4 = 1;
    SSPSTAT = 0b10000000;
    SSPCON = 0b00010001;
    SSPEN = 1;
}

void SPIInitSlave()
{
    TRISB4 = 1;
    SSPSTAT = 0b00000000;
    SSPCON = 0b00010101;
    SSPEN = 1;
}

int SPIWrite(int SPImessage)
{
    SSPBUF = SPImessage;
    while(!BF);
    return SSPBUF;
}


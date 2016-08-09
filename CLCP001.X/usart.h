/* USART Settings Header */

#define BAUD 2400      
#define FOSC 8000000L
#define baudsetting ((int)(FOSC/(64UL * BAUD) -1))

#define RX_PIN TRISB5
#define TX_PIN TRISB7

/* Prototypes */
void putch(unsigned char);
unsigned char getch(void);
unsigned char getche(void);
void init_comms(void);



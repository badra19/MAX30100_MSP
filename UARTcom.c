#include <msp430.h>

void initUart();
void sendData(unsigned char c);
void sendInt(unsigned int n);
void sendString(char str[]);

void initUart() // 115200 Baudrate
{
    // Configure UART
    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;

    // Baud Rate calculation
    UCA0BR0 = 8;
    UCA0BR1 = 0x00;
    UCA0MCTLW = 0xF700 | UCOS16 | UCBRF_10;

    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    //UCA0IE |= UCRXIE;                // Enable USCI_A0 RX interrupt
}

void sendData(unsigned char c)
{
    while(!(UCA0IFG&UCTXIFG));
    UCA0TXBUF = c;
}

void sendInt(unsigned int n)
{
    int num[] = {10000,1000,100,10,1 };
    int pos, posI;

    if(n==0)
    {
        sendData('0');
        return;
    }

    for (pos = posI; pos < 5; pos++)
    {
      int cnt = 0;
      while (n >= num[pos]) {
        cnt++;
        n -= num[pos];
      }
      sendData(cnt+'0');
    }
}

void sendString(char str[])
{
    unsigned int i;
    for(i=0; str[i]!= '\0'; i++)
        sendData(str[i]);
}

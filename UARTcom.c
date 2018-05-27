#include <msp430.h>

void initUart();
void sendData(unsigned char c);
void sendInt(int n);
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

void sendInt(int n)
{
    int casa, dig;
    if(n==0)
    {
        sendData('0');
        return;
    }
    if(n<0)
    {
        sendData('-');
        n = -n;
    }

    for(casa = 1; casa<=n; casa *= 10);
    casa /= 10;
    while(casa>0)
    {
        dig = (n/casa);
        sendData(dig+'0');
        n -= dig*casa;
        casa /= 10;
    }

}

void sendString(char str[])
{
    unsigned int i;
    for(i=0; str[i]!= '\0'; i++)
        sendData(str[i]);
}

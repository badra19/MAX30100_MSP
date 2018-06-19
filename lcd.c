#include <msp430fr2433.h>

#define LCD_OUT P2OUT
#define LCD_DIR P2DIR
#define D4 BIT4
#define D5 BIT5
#define D6 BIT6
#define D7 BIT7
#define RS BIT0
#define E  BIT1
#define DADOS 1
#define COMANDO 0
#define CMND_DLY 1000
#define DATA_DLY 1000
#define BIG_DLY  2900
#define CLR_DISPLAY  sendByteLCD(1, COMANDO, BIG_DLY)
#define POS0_DISPLAY sendByteLCD(2, COMANDO, BIG_DLY)

void delayUs(volatile unsigned int us);
void sendNibleLCD(volatile unsigned char nibble, volatile unsigned char dados, volatile unsigned int microsegs);
void sendByteLCD(volatile unsigned char byte, volatile unsigned char dados, volatile unsigned int microsegs);
void sendDataLCD(volatile unsigned char byte);
void sendStringLCD(char str[]);
void sendIntLCD(int n);
void sendFloatLCD(volatile float var_float);
void initLCD(void);

void initLCD(void)
{
    unsigned char CMNDS[] = {0x02, 0x01, 0x28, 0x0C};
    unsigned int i;
    // Atraso de 10ms para o LCD fazer o boot
    delayUs(10000);
    LCD_DIR |= D4+D5+D6+D7+RS+E;
    Send_Nibble(0x03, COMANDO, CMND_DLY);
    for(i=0; i<4; i++)
        sendByteLCD(CMNDS[i], COMANDO, CMND_DLY);
    CLR_DISPLAY;
    POS0_DISPLAY;
}

void delayUs(volatile unsigned int us)
{
    TA1CCR0 = us-1;
    TA1CTL = TASSEL_2 + ID_3 + MC_3;
    while((TA1CTL & TAIFG)==0);
    TA1CTL = TACLR;
    TA1CTL = 0;
}

void Send_Nibble(volatile unsigned char nibble, volatile unsigned char dados, volatile unsigned int microsegs)
{
    LCD_OUT |= E;
    LCD_OUT &= ~(RS + D4 + D5 + D6 + D7);
    LCD_OUT |= RS*(dados==DADOS) +
        D4*((nibble & BIT0)>0) +
        D5*((nibble & BIT1)>0) +
        D6*((nibble & BIT2)>0) +
        D7*((nibble & BIT3)>0);
    LCD_OUT &= ~E;
    delayUs(microsegs);
}

void sendByteLCD(volatile unsigned char byte, volatile unsigned char dados, volatile unsigned int microsegs)
{
    Send_Nibble(byte >> 4, dados, microsegs/2);
    Send_Nibble(byte & 0xF, dados, microsegs/2);
}

void sendDataLCD(volatile unsigned char byte)
{
    sendByteLCD(byte, DADOS, DATA_DLY);
}

void sendStringLCD(char str[])
{
    while((*str)!='\0')
    {
        sendDataLCD(*(str++));
    }
}

void sendIntLCD(int n)
{
    int casa, dig;
    if(n==0)
    {
        sendDataLCD('0');
        return;
    }
    if(n<0)
    {
        sendDataLCD('-');
        n = -n;
    }
    for(casa = 10000; casa>n; casa /= 10);
    while(casa>0)
    {
        dig = (n/casa);
        sendDataLCD(dig+'0');
        n -= dig*casa;
        casa /= 10;
    }
}

void sendFloatLCD(volatile float var_float)
{
    volatile int var_int;

    if (var_float < 0) // se for negativo
    {
        var_float = var_float * (-1); // multiplica por -1
        sendDataLCD('-'); // imprime sinal negativo
    }

    var_int = (int) var_float; // converte para inteiro
    sendIntLCD(var_int); // envia parte inteira

    delayUs(2);
    sendDataLCD('.'); // envia o "."

    var_float = (var_float - var_int)*100; // multiplica a parte residual não inteira
    var_int = (int) var_float; // converte as duas primeiras casas decimais em inteiro
    sendIntLCD(var_int); // envia as duas primeiras casas decimais
}



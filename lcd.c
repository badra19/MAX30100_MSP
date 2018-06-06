#include <msp430fr2433.h>

#define BTN BIT3
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
#define BIG_DLY  3000
#define CLR_DISPLAY  Send_Byte(1, COMANDO, BIG_DLY)
#define POS0_DISPLAY Send_Byte(2, COMANDO, BIG_DLY)

void Atraso_us(volatile unsigned int us);
void Send_Nibble(volatile unsigned char nibble, volatile unsigned char dados, volatile unsigned int microsegs);
void Send_Byte(volatile unsigned char byte, volatile unsigned char dados, volatile unsigned int microsegs);
void Send_Data(volatile unsigned char byte);
void Send_String(char str[]);
void Send_Int(int n);
void Send_Float(volatile float var_float);
void InitLCD(void);

int main(void)
{
	float oi = 0;
	WDTCTL = WDTHOLD + WDTPW;

	PM5CTL0 &= ~LOCKLPM5;

	CSCTL1 = 0xF1;      //MCLK e SMCLK @ 1MHz
    CSCTL0 = 0x00;

	P2DIR &= ~BTN;
	P2OUT = P2REN = BTN;
	InitLCD();
	Send_String("Pressione BTN");
	while(1)
	{
		// Espera o botao ser pressionado
		while(P2IN&BTN);
		// Espera o botao ser solto
		while((P2IN&BTN)==0);
		// Transmite a string
		CLR_DISPLAY;
		POS0_DISPLAY;
		Send_Float(oi+= 0.1);
		Send_String(": Ola SS");
	}
	return 0;
}

void InitLCD(void)
{
	unsigned char CMNDS[] = {0x02, 0x01, 0x28, 0x0E};
	unsigned int i;
	// Atraso de 10ms para o LCD fazer o boot
	Atraso_us(10000);
	LCD_DIR |= D4+D5+D6+D7+RS+E;
	Send_Nibble(0x03, COMANDO, CMND_DLY);
	for(i=0; i<4; i++)
		Send_Byte(CMNDS[i], COMANDO, CMND_DLY);
	CLR_DISPLAY;
	POS0_DISPLAY;
}

void Atraso_us(volatile unsigned int us)
{

	TA1CCR0 = us-1;
	TA1CTL = TASSEL_2 + ID_3 + MC_3 + TAIE;
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
	Atraso_us(microsegs);
}

void Send_Byte(volatile unsigned char byte, volatile unsigned char dados, volatile unsigned int microsegs)
{
	Send_Nibble(byte >> 4, dados, microsegs/2);
	Send_Nibble(byte & 0xF, dados, microsegs/2);
}

void Send_Data(volatile unsigned char byte)
{
	Send_Byte(byte, DADOS, DATA_DLY);
}

void Send_String(char str[])
{
	while((*str)!='\0')
	{
		Send_Data(*(str++));
	}
}

void Send_Int(int n)
{
	int casa, dig;
	if(n==0)
	{
		Send_Data('0');
		return;
	}
	if(n<0)
	{
		Send_Data('-');
		n = -n;
	}
	for(casa = 10000; casa>n; casa /= 10);
	while(casa>0)
	{
		dig = (n/casa);
		Send_Data(dig+'0');
		n -= dig*casa;
		casa /= 10;
	}
}

void Send_Float(volatile float var_float)
{
    volatile int var_int;

    if (var_float < 0) // se for negativo
    {
        var_float = var_float * (-1); // multiplica por -1
        Send_Data('-'); // imprime sinal negativo
    }

	var_int = (int) var_float; // converte para inteiro
	Send_Int(var_int); // envia parte inteira

	Atraso_us(2);
	Send_Data('.'); // envia o "."

	var_float = (var_float - var_int)*100; // multiplica a parte residual não inteira
	var_int = (int) var_float; // converte as duas primeiras casas decimais em inteiro
	Send_Int(var_int); // envia as duas primeiras casas decimais
}



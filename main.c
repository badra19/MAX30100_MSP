#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "MAX30100.c"
#include "UARTcom.c"
#include "MAX30100_PulseOximeter.c"
#include "lcd.c"

//******************************************************************************
// Pin Config ******************************************************************
//******************************************************************************

#define LED_OUT     P1OUT
#define LED_DIR     P1DIR
#define LED0_PIN    BIT0
#define LED1_PIN    BIT1

//******************************************************************************
// Device Initialization *******************************************************
//******************************************************************************


void initGPIO()
{
    // Configure GPIO
    LED_OUT &= ~(LED0_PIN | LED1_PIN); // P1 setup for LED & reset output
    LED_DIR |= (LED0_PIN | LED1_PIN);

    // I2C pins
    P1SEL0 |= BIT2 | BIT3;
    P1SEL1 &= ~(BIT2 | BIT3);

    // UART pins
    P1SEL0 |= BIT4 | BIT5; // set 2-UART pin as second function

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
}

void initClockTo16MHz()
{
    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    // operation beyond 8MHz _before_ configuring the clock system.
    FRCTL0 = FRCTLPW | NWAITS_1;

    // Clock System Setup
    __bis_SR_register(SCG0);                           // disable FLL
    CSCTL3 |= SELREF__REFOCLK;                         // Set REFO as FLL reference source
    CSCTL0 = 0;                                        // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);                            // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_5;                               // Set DCO = 16MHz
    CSCTL2 = FLLD_0 + 487;                             // DCOCLKDIV = 16MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                           // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));         // FLL locked
}

//******************************************************************************
// Main ************************************************************************
//******************************************************************************

int main(void) {
    initWDT();
    initClockTo16MHz();
    initGPIO();
    InitLCD();
    initI2C();
    initUart();
    __bis_SR_register(GIE);

    sendString("Initializing MAX30100..");
    sendData('\n');
    if(pulseOxBegin(PULSEOXIMETER_DEBUGGINGMODE_NONE) == true)
    {
        LED_OUT = LED0_PIN;
        sendString("Sucess");
        sendData('\n');
    }
    else

    {
        LED_OUT = LED0_PIN + LED1_PIN;
        sendString("Failed");
        sendData('\n');
        return 1;
    }

    unsigned int spo2Read, hrRead;
    while(1)
    {
        pulseOxUpdate();
        spo2Read = (unsigned int) pulseOxGetSpO2();
        hrRead = (unsigned int) pulseOxGetHeartRate();

        CLR_DISPLAY;
        POS0_DISPLAY;
        sendStringLCD("SpO2: ");
        sendIntLCD((int) spo2Read);
        sendStringLCD(" HR: ");
        sendIntLCD((int) hrRead);
        delayUs(0xFFFF);

        /*
        sendString("SpO2: ");
        sendInt(spo2Read);
        sendData('\t');
        sendString("HR: ");
        sendFloat(hrRead);
        sendData('\n');
        */
    }

    return 0;
}

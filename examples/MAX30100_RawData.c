#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "MAX30100.c"
#include "UARTcom.c"

//******************************************************************************
// Pin Config ******************************************************************
//******************************************************************************

#define LED_OUT     P1OUT
#define LED_DIR     P1DIR
#define LED0_PIN    BIT0
#define LED1_PIN    BIT1

#define SAMPLING_RATE                       MAX30100_SAMPRATE_100HZ
#define IR_LED_CURRENT                      MAX30100_LED_CURR_50MA
#define RED_LED_CURRENT                     MAX30100_LED_CURR_27_1MA
#define PULSE_WIDTH                         MAX30100_SPC_PW_1600US_16BITS
#define HIGHRES_MODE                        true

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
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    initClockTo16MHz();
    initGPIO();
    initI2C();
    initUart();
    __bis_SR_register(GIE);

    sendString("Initializing MAX30100..");
    if(begin() == true)
    {
        LED_OUT = LED0_PIN;
        sendString("Sucess");
    }
    else
    {
        LED_OUT = LED0_PIN + LED1_PIN;
        sendString("Failed");
        return 1;
    }

    setMode(MAX30100_MODE_SPO2_HR);
    setLedsCurrent(IR_LED_CURRENT, RED_LED_CURRENT);
    setLedsPulseWidth(PULSE_WIDTH);
    setSamplingRate(SAMPLING_RATE);
    setHighresModeEnabled(HIGHRES_MODE);

    sendString("Start Samples");
    uint16_t ir, red;
    while(1)
    {
        update();

        while(getRawValues(&ir, &red))
        {
            sendInt((unsigned int) ir);
            sendData('\t');
            sendInt((unsigned int) red);
            sendData('\n');
            __delay_cycles(100000);
        }
    }

    return 0;
}

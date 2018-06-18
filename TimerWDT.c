#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

uint32_t millis(void);
uint32_t _millis = 0;

void initWDT();
void holdWDT();
void clearMillis();


void initWDT()
{
    WDTCTL = WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2; // Interrupt every 2 ms. 16MHz Clk
    // WDTCTL = WDT_MDLY_32;
    //          WDT_MDLY_0_5;
    SFRIE1 |= WDTIE;
}

void holdWDT()
{
    WDTCTL = WDTPW | WDTHOLD | WDTCNTCL;
}

uint32_t millis(void)
{
  uint32_t safe_millis;
  __disable_interrupt();
  safe_millis = _millis;
  __enable_interrupt();
  return safe_millis;
}

void clearMillis()
{
  __disable_interrupt();
  _millis = 0;
  __enable_interrupt();
} 

// Watchdog Timer interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) WDT_ISR (void)
#else
#error Compiler not supported!
#endif
{
  _millis += 2;
}

#include "pump.h"

#include "tc.h"  // Timer
#include "pmc.h" // Power Managment
#include "io.h"  // IO --> with user defined function
#include "aic.h"

int pumpState = 0;

///
///
///
///
void init_pump(void)
{
    StructTC *timerbase3 = TCB3_BASE; // Baseadresse TC Block1
    StructPIO *piobaseA = PIOA_BASE;  // Baseadresse PIO B

    timerbase3->TC_CCR = TC_CLKDIS; // Disable Clock

    // Initialize the mode of the timer 3
    timerbase3->TC_CMR =
        TC_ACPC_TOGGLE_OUTPUT | //ACPC    : Register C TOGGLE TIOA
        TC_WAVE |               //WAVE    : Waveform mode
        TC_CPCTRG |             //CPCTRG  : Register C compare trigger enable
        TC_CLKS_MCK8;           //TCCLKS  : MCKI / 8

    // Initialize the counter:
    timerbase3->TC_RC = 31250; // Counter Reset 31250 = 25.000.000 (25Mhz) / 8(Teiler) / 50(Hz) / 2(H?lfte an h?lfte aus)

    piobaseA->PIO_PER = (1 << PIOTIOA3); // Keep safe from permanent high
}

///
///
///
void enablePump(void)
{
    StructPIO *piobaseA = PIOA_BASE;
    StructTC *timerbase3 = TCB3_BASE;

    timerbase3->TC_CCR = TC_CLKEN | TC_SWTRG;
    piobaseA->PIO_PDR = (1 << PIOTIOA3); // Enable pump by disable IO Control
}

///
///
///
void disablePump(void)
{
    StructPIO *piobaseA = PIOA_BASE;
    StructTC *timerbase3 = TCB3_BASE;

    piobaseA->PIO_PER = (1 << PIOTIOA3); // Keep safe from permanent high signal
    timerbase3->TC_CCR = TC_CLKDIS;
}
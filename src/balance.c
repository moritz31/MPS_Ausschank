#include "balance.h"

#include "pio.h"
#include "pmc.h"
#include "tc.h"

#define TC4_INIT TC_CLKS_MCK2 | TC_LDBSTOP | TC_CAPT | TC_LDRA_RISING_EDGE | TC_LDRB_RISING_EDGE // MCKI/2, Counter Clock stoppen wenn RB l?dt, Capture Mode, RA Capture Rising, RB Capture Rising
#define TC5_INIT TC_CLKS_MCK2 | TC_LDBSTOP | TC_CAPT | TC_LDRA_RISING_EDGE | TC_LDRB_RISING_EDGE // MCKI/2, Counter Clock stoppen wenn RB l?dt, Capture Mode, RA Capture Rising, RB Capture Rising

#define PA4 (1 << 4)
#define PA7 (1 << 7)

///
///
///
///
void init_balance(void)
{

    // Zeiger auf Timer und PIO holen
    StructPIO *piobaseA = PIOA_BASE;
    StructTC *tcbase4 = TCB4_BASE;
    StructTC *tcbase5 = TCB5_BASE;

    piobaseA->PIO_PDR = PA4 | PA7; // PA7 und PA4 ausschalten

    // TIOA4 initalisieren
    tcbase4->TC_CCR = TC_CLKDIS; // Clock ausschalten
    tcbase4->TC_CMR = TC4_INIT;
    tcbase4->TC_CCR = TC_CLKEN; // Clock einschalten
    tcbase4->TC_CCR = TC_SWTRG; // Trigger

    // TIOA5 initalisieren
    tcbase5->TC_CCR = TC_CLKDIS; // Clock ausschalten
    tcbase5->TC_CMR = TC5_INIT;
    tcbase5->TC_CCR = TC_CLKEN; // Clock einschalten
    tcbase5->TC_CCR = TC_SWTRG; // Trigger
}

///
///
///
///
int measure(void)
{

    // Initalize local variable
    volatile int captureRA = 0;
    volatile int captureRB = 0;
    volatile float period1 = 0;
    volatile float period2 = 0;

    // Get pointer to the structs
    StructTC *tcbase4 = TCB4_BASE;
    StructTC *tcbase5 = TCB5_BASE;

    // Activate both software trigger
    tcbase4->TC_CCR = TC_SWTRG; // Trigger
    tcbase5->TC_CCR = TC_SWTRG; // Trigger

    // We want to read TIOA4 first, because the frequency raises with more weight and therefor the measurement works faster

    // Read TIOA4

    /* !!! BUSY WAITING !!! */
    while (!(tcbase4->TC_SR & 0x40))
        ; // Capture Register B was loaded
    captureRA = tcbase4->TC_RA;
    captureRB = tcbase4->TC_RB;
    period1 = captureRB - captureRA; // Difference between RA(RISING) RB(RISING)

    // Read TIOA5

    /* !!! BUSY WAITING !!! */
    while (!(tcbase5->TC_SR & 0x40))
        ; // Capture Regier B was loaded
    captureRA = tcbase5->TC_RA;
    captureRB = tcbase5->TC_RB;
    period2 = captureRB - captureRA; // Difference between RA(RISING) RB(RISING)

    // Calculate weight and return it
    return calculateMass(0, 2000, period1, period2);
}

///
///
///
///
int calculateMass(int c1 ,int c2, int p1, int p2) {
    //Calculate weight m = C1 * ((TIOA4/TIOA5) - 1) - C2 
    return c2 * ((p1/p2) -1) - c1;
}
#include "balance.h"

#include "pio.h"
#include "pmc.h"
#include "tc.h"

#define TC4_INIT  TC_CLKS_MCK2 | TC_LDBSTOP | TC_CAPT | TC_LDRA_RISING_EDGE | TC_LDRB_RISING_EDGE // MCKI/2, Counter Clock stoppen wenn RB l?dt, Capture Mode, RA Capture Rising, RB Capture Rising 
#define TC5_INIT  TC_CLKS_MCK2 | TC_LDBSTOP | TC_CAPT | TC_LDRA_RISING_EDGE | TC_LDRB_RISING_EDGE // MCKI/2, Counter Clock stoppen wenn RB l?dt, Capture Mode, RA Capture Rising, RB Capture Rising

#define PA4 (1<<4)
#define PA7 (1<<7)

///
///
///
///
void init_balance( void ) {

    // Zeiger auf Timer und PIO holen
    StructPIO* piobaseA = PIOA_BASE;
    StructTC* tcbase4   = TCB4_BASE;
    StructTC* tcbase5   = TCB5_BASE;
    
    piobaseA->PIO_PDR	=	PA4 | PA7;	  // PA7 und PA4 ausschalten
    
    // TIOA4 initalisieren
    tcbase4->TC_CCR		=	TC_CLKDIS; // Clock ausschalten
    tcbase4->TC_CMR		=	TC4_INIT;
    tcbase4->TC_CCR		=	TC_CLKEN; // Clock einschalten
    tcbase4->TC_CCR		=	TC_SWTRG; // Trigger
        
    // TIOA5 initalisieren
    tcbase5->TC_CCR		=	TC_CLKDIS; // Clock ausschalten
    tcbase5->TC_CMR		=	TC5_INIT;
    tcbase5->TC_CCR		=	TC_CLKEN; // Clock einschalten
    tcbase5->TC_CCR		=	TC_SWTRG; // Trigger
}
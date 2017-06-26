#include "irq.h"
#include "io.h"
#include "aic.h"

#define AIC_PIOB (1 << 14)
#define AIC_PIOB_ID 14

///
///
///
void irq_register(void (*irq)())
{
    StructAIC* aicbase = AIC_BASE;
    StructPIO* piobaseB = PIOB_BASE;

    aicbase->AIC_IDCR = AIC_PIOB; // PIOB IRQ disable
    aicbase->AIC_ICCR = AIC_PIOB; // PIOB IRQ delete

    aicbase->AIC_SVR[AIC_PIOB_ID] = (unsigned int)irq;  // ISR register
    aicbase->AIC_SMR[AIC_PIOB_ID] = 0x7;                // ISR priority

    aicbase->AIC_IECR = AIC_PIOB;    // PIOB IRQ activation
    piobaseB->PIO_IER = KEY1 | KEY2; // SW1 and SW2 as interrupt source
}
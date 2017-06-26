#include "io.h"
#include "seriell.h"
#include "balance.h"
#include "pump.h"
#include "irq.h"
#include "global_config.h"
#include "aic.h"

char *welcomeMsg = "Welcome to MPS #6 \n";

/**
 * 0 = Welcome message
 * 1 = Setup 
 * 2 = Setup_finished
 * 3 = Weight
 * 4 = Endprocedure
 */
int global_state = 0;

/**
 * @brief Interrupt ISR which handles the key input for SW1 and SW2
 * @author Moritz Weichert, Phil Goeritz
 */
void interrupt_isr(void) __attribute__((interrupt));

/**
 * @brief Mainfunction
 * @author Moritz Weichert, Phil Goeritz
 * @return Errorcode
 */
int main()
{

    StructPIO *pioBaseB = PIOB_BASE;

    /**
     * 1. Init all stuff
     */

    // Activate SW1/SW2 as Input
    initInput(pioBaseB, KEY1);
    initInput(pioBaseB, KEY2);
    // Register IRQ
    //irq_register(interrupt_isr);
    // Init serial connection
    init_ser();
    // Init balance
    init_balance();
    // Init pump
    init_pump();

    int cupWeight;
    int totalWeight;
    while (1)
    {

        
        
        switch (global_state)
        {
        case 0:
            // 2. Put Welcome and Information over serial
            puts(welcomeMsg);
            break;

        case 1:
            // 3. Weight, calib and show cup weight

            cupWeight = 0; // Set weight to zero
            /* BUSY Waiting */
            while (global_state != 1)
                ;                      // Wait while cup gets on the balance and key gets pressed
            cupWeight = measure(); // Measure the weight
            puts("The cup weights: "); // Print the message
            puts((char)cupWeight);     // Print the weight casted to a character
            global_state = 2;
            break;

        case 3:
            // 4. Fill up until target is reached
            
            totalWeight = 0;
            enablePump();
            global_state = 4; // After filling up the next state is 4
            while(totalWeight <= TARGET_WEIGHT) {
                totalWeight = measure() - cupWeight; // Measured weight - weight of the cup

                // If totalweight is less then zero the cup is taken away
                if(totalWeight < 0 ) {
                    global_state = 0;
                    puts("Becher weggenommen; Beginne von vorne.");
                    break;
                } 
            }
            disablePump();

            break;
        case 4:
            puts((char)TARGET_WEIGHT);
            puts(" abgefüllt \n");
            puts("Sie können den Becher nun entfernen!\n");
            /* BUSY WAITING */
            while (measure >= 0)
                ; // Wait until cup gets removed
            global_state = 2;
            break;

        default:
            // Default for states in which nothing should happen
            break;
        }
    }

    return 0;
}

///
///
///
void interrupt_isr(void)
{
    StructPIO *piobaseB = PIOB_BASE;
    StructAIC *aicbase = AIC_BASE;

    // SW1 Input for cup calibration
    if (getInput(piobaseB, KEY1) && global_state == 0)
    {
        global_state = 1;
    }

    // SW2 Input for start pump after calibration
    if (getInput(piobaseB, KEY2) && global_state == 2)
    {
        global_state = 3;
    }

    aicbase->AIC_EOICR = piobaseB->PIO_ISR; // End of Interrupt
}
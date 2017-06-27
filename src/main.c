#include "io.h"
#include "seriell.h"
#include "balance.h"
#include "pump.h"
#include "irq.h"
#include "global_config.h"
#include "aic.h"
#include "pmc.h"


#define PCER_PIOA  (1<<12)
#define PCER_PIOB  (1<<13)
#define PCER_TC5   (1<<10)
#define PCER_TC4   (1<<9)
#define PCER_TC3   (1<<8)
#define PCER_TC2   (1<<7)
#define PCER_TC1   (1<<6)

char *welcomeMsg = "Willkommen im 6. Praktikum von MPS \n";
char buffer[10] = { 0 };

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


void reverse(char s[], int length) {
    
    char tmp;
    int i,j;
    
    for(i = 0, j = length - 1; i < j; i++, j--) {
        tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;
    }
}

 /* itoa:  convert n to characters in s */
 void itoa(int n, char s[])
 {   
     if(n == 0x80000000) {
      s = "-2147483648";
      puts(s);
      return;
     }
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s,i);
     puts(s);
 }

/**
 * @brief Mainfunction
 * @author Moritz Weichert, Phil Goeritz
 * @return Errorcode
 */
int main()
{
    itoa(0,buffer);
    StructPIO *pioBaseB = PIOB_BASE;
    StructPIO *pioBaseA = PIOA_BASE;
    StructPMC *pmcbase = PMC_BASE;

    /**
     * 1. Init all stuff
     */

    pmcbase->PMC_PCER = 0x6f80;
    pioBaseA->PIO_PDR = 0x090;
    
    // Activate SW1/SW2 as Input
    initInput(pioBaseB, KEY1);
    initInput(pioBaseB, KEY2);
    // Register IRQ
    irq_register(interrupt_isr);
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
            global_state = 1;
            break;

        case 1:
            // 3. Weight, calib and show cup weight

            cupWeight = 0; // Set weight to zero
            /* BUSY Waiting */
            while (global_state != 2)
                ;                      // Wait while cup gets on the balance and key gets pressed
            cupWeight = measure(); // Measure the weight
            puts("Der Becher wiegt: "); // Print the message
            itoa(cupWeight,buffer);     // Print the weight casted to a character
            puts("\n");
            global_state = 2;
            break;

        case 3:
            // 4. Fill up until target is reached
            
            totalWeight = 0;
            enablePump();
            global_state = 4; // After filling up the next state is 4
            int lastWeight = 0;
            int cntr = 0;
            lastWeight = totalWeight;
            while(totalWeight < TARGET_WEIGHT) {
                totalWeight = measure() - cupWeight; // Measured weight - weight of the cup

                if(lastWeight < totalWeight)
                    cntr = 0;
                else 
                    cntr++;
                
                if(cntr >= 1000) {
                    puts("Moggeldetektion aktiviert! \n");
                    global_state = 0;
                    break;
                }
                
                // If totalweight is less then zero the cup is taken away
                if(totalWeight < 0 ) {
                    global_state = 0;
                    puts("Becher weggenommen; Beginne von vorne. \n");
                    break;
                }
                
                lastWeight = totalWeight; // Set lastWeight to totalWeight
            }
            disablePump();

            break;
        case 4:
            itoa(TARGET_WEIGHT,buffer);
            puts(" abgefuellt \n");
            puts("Sie koennen den Becher nun entfernen!\n");
            /* BUSY WAITING */
            while (measure() >= 1)
                ; // Wait until cup gets removed
            global_state = 0;
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
    if (getInput(piobaseB, KEY1) && global_state == 1)
    {
        global_state = 2;
    }

    // SW2 Input for start pump after calibration
    if (getInput(piobaseB, KEY2) && global_state == 2)
    {
        global_state = 3;
    }

    aicbase->AIC_EOICR = piobaseB->PIO_ISR; // End of Interrupt
}
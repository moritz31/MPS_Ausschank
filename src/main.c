#include "io.h"
#include "seriell.h"
#include "balance.h"
#include "pump.h"
#include "global_config.h"

char* welcomeMsg = "Welcome to MPS #6 \n";

/**
 * @brief Mainfunction
 * @author Moritz Weichert, Phil Goeritz
 * @return Errorcode
 */
int main() {

    StructPIO* pioBaseB = PIOB_BASE;

    /**
     * 1. Init all stuff
     */

    // Init serial connection
    init_ser(); 
    // Init balance
    init_balance();
    // Init pump
    init_pump();

    // Activate SW1/SW2 as Input
    initInput(pioBaseB,KEY1);
    initInput(pioBaseB,KEY2);
    

    // 2. Put Welcome and Information over serial
    puts(welcomeMsg);

    // 3. Weight, calib and show cup weight

    /* BUSY Waiting */
    while(!getInput(pioBaseB,KEY1)); // Wait while cup gets on the balance and key gets pressed
    int cupWeight = measure(); // Measure the weight
    puts("The cup weights: "); // Print the message
    puts((char)cupWeight); // Print the weight casted to a character

    while(1) {

        // 4. Fill the cup if key2 gets pressed
        if(getInput(pioBaseB,KEY2)) {
            int notFinished = 1;
            switchPump(); // Enables the pump
            while(notFinished) {
                int weight = measure() - cupWeight;
                if(cupWeight >= TARGET_WEIGHT) { // If we reached the weight exit the loop
                    notFinished = 0;
                    break;
                }
            }
            switchPump(); // Turn pump off
        }

    }

    return 0;
}